#include <sync/spinlock.h>
#include <smp/pcpu.h>
#include <util/list.h>
#include <mm/mm.h>
#include <util/def.h>
#include <arch/intrin.h>
#include <proc/thread.h>
#include <proc/sched.h>

#include "event.h"

static tpl_t early_current_tpl = TPL_HIGH_LEVEL;

//----------------------------------------------------------------------------------------------------------------------
// Event spinlock
//----------------------------------------------------------------------------------------------------------------------

static spinlock_t event_lock = SPINLOCK_INIT;

static void aquire_event_lock() {
    spinlock_acquire_high_tpl(&event_lock);
}

static void release_event_lock() {
    spinlock_release(&event_lock);
}

//----------------------------------------------------------------------------------------------------------------------
// Event data
//----------------------------------------------------------------------------------------------------------------------

typedef struct event_data {
    // notify function info
    tpl_t notify_tpl;
    notify_function_t notify_function;
    void* notify_ctx;

    // thread waiting for the event
    thread_t* thread;

    // the signal
    atomic_bool signaled;

    // this is for the notify list
    list_entry_t notify_link;
} event_data_t;

// TODO: non-busy waiting for events

/**
 * This is for events with a notification function that need to be dispatched
 */
static list_entry_t notify_queues[TPL_HIGH_LEVEL + 1];

/**
 * Bitmask of notify lists with events
 */
static uint32_t notify_queues_pending = 0;

void events_init() {
    // set the notify dispatch list entries
    for (int i = 0; i < ARRAY_LEN(notify_queues); i++) {
        notify_queues[i] = INIT_LIST_ENTRY(notify_queues[i]);
    }
}

static bool dispatch_event_notifies(tpl_t tpl) {
    bool had_any = false;
    aquire_event_lock();
    ASSERT(event_lock.owner_tpl == tpl);
    list_entry_t* list = &notify_queues[tpl];

    while (!is_list_empty(list)) {
        had_any = true;

        // pop the data
        event_data_t* data = CR(list->next, event_data_t, notify_link);
        remove_entry_list(&data->notify_link);
        data->notify_link.next = NULL;

        // clear the signal bool
        data->signaled = false;

        // notify it outside of the event lock
        release_event_lock();
        WARN(!IS_ERROR(data->notify_function(data->notify_ctx, data)), "Notify function returned an error");
        aquire_event_lock();
    }

    notify_queues_pending &= ~(1u << tpl);
    release_event_lock();

    return had_any;
}

//----------------------------------------------------------------------------------------------------------------------
// TPL implementation
//----------------------------------------------------------------------------------------------------------------------

tpl_t raise_tpl(tpl_t new_tpl) {
    tpl_t old_tpl = get_tpl();

    // can not raise to lower tpl
    ASSERT(old_tpl <= new_tpl);

    // if raising to high level disable interrupts
    if (new_tpl >= TPL_HIGH_LEVEL && old_tpl < TPL_HIGH_LEVEL) {
        asm volatile("cli" ::: "memory");
    }

    set_tpl(new_tpl);

    return old_tpl;
}

void restore_tpl(tpl_t new_tpl) {
    tpl_t old_tpl = get_tpl();

    // can not lower to higher tpl
    ASSERT_TRACE(new_tpl <= old_tpl, "Switching from tpl %d to tpl %d is invalid!", old_tpl, new_tpl);

    if (old_tpl >= TPL_HIGH_LEVEL && new_tpl < TPL_HIGH_LEVEL) {
        set_tpl(TPL_HIGH_LEVEL);
    }

    // dispatch events
    while (notify_queues_pending) {
        // get the next tpl with pending events
        tpl_t pending_tpl;
        _BitScanReverse(&pending_tpl, notify_queues_pending);
        if (pending_tpl <= new_tpl) {
            break;
        }


        // dispatch it
        set_tpl(pending_tpl);
        if (pending_tpl < TPL_HIGH_LEVEL) {
            asm volatile("sti" ::: "memory");
        }
        dispatch_event_notifies(pending_tpl);
    }

    // set the tpl
    set_tpl(new_tpl);
    if (new_tpl < TPL_HIGH_LEVEL) {
        asm volatile("sti" ::: "memory");
    }
}

tpl_t get_tpl() {
    // before threading use a static variable
    if (UNLIKELY(current_thread == NULL)) {
        return early_current_tpl;
    } else {
        return current_thread->thread_tpl;
    }
}

void set_tpl(tpl_t tpl) {
    // before threading use a static variable
    if (UNLIKELY(current_thread == NULL)) {
        early_current_tpl = tpl;
    } else {
        current_thread->thread_tpl = tpl;
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Event implementation
//----------------------------------------------------------------------------------------------------------------------

err_t create_event(tpl_t notify_tpl, notify_function_t notify_function, void* notify_ctx, event_t* event) {
    err_t err = NO_ERROR;

    CHECK_ERROR(event != NULL, ERROR_INVALID_PARAM);
    *event = NULL;

    event_data_t* data = mm_allocate(sizeof(event_data_t));
    CHECK_ERROR(data != NULL, ERROR_OUT_OF_RESOURCES);

    data->notify_tpl = notify_tpl;
    data->notify_function = notify_function;
    data->notify_ctx = notify_ctx;

    data->signaled = false;

    *event = data;

cleanup:
    return err;
}

err_t signal_event(event_t event) {
    err_t err = NO_ERROR;
    event_data_t* data = event;

    CHECK_ERROR(event != NULL, ERROR_INVALID_PARAM);

    aquire_event_lock();

    if (!data->signaled) {
        data->signaled = true;

        // if got a notification function then add it to dispatch queue
        if (data->notify_function != NULL) {
            insert_tail_list(&notify_queues[data->notify_tpl], &data->notify_link);
            notify_queues_pending |= (1u << data->notify_tpl);
        }

        // if got a thread then reschedule it
        if (data->thread != NULL) {
            // queue the thread
            CHECK_AND_RETHROW(queue_thread(data->thread));
            data->thread = NULL;
        }
    }

    release_event_lock();

cleanup:
    return err;
}

err_t check_event(event_t event) {
    err_t err = NO_ERROR;
    event_data_t* data = event;

    CHECK_ERROR(event != NULL, ERROR_INVALID_PARAM);

    bool yes = true;
    err = atomic_compare_exchange_weak_explicit(&data->signaled, &yes, false, memory_order_consume, memory_order_consume) ? NO_ERROR : ERROR_NOT_READY;

cleanup:
    return err;
}

err_t wait_for_event(size_t number_of_events, event_t* events, size_t* index) {
    err_t err = NO_ERROR;

    // this is so we are always gonna yield in TPL_APPLICATION
    CHECK_ERROR_TRACE(get_tpl() == TPL_APPLICATION, ERROR_INVALID_TPL, "Current TPL %d", get_tpl());

    CHECK_ERROR(events != NULL, ERROR_INVALID_PARAM);
    CHECK_ERROR(number_of_events > 0, ERROR_INVALID_PARAM);

    while(1) {
        for (size_t i = 0; i < number_of_events; i++) {
            err = check_event(events[i]);
            if (err == NO_ERROR) {
                if(index != NULL) *index = i;
                goto cleanup;
            } else if (err == ERROR_NOT_READY) {
                // do nothing
            } else {
                CHECK_AND_RETHROW(err);
            }

            // add the thread as waiting for the event
            event_data_t* data = events[i];
            CHECK_ERROR(data->notify_function == NULL, ERROR_INVALID_PARAM);  // can not wait for event with
                                                                                    // callback function
            CHECK_ERROR(data->thread == NULL || data->thread == current_thread, ERROR_NOT_READY);
            data->thread = current_thread;
        }

        // set the thread as waiting and yield
        current_thread->state = STATE_WAITING;
        yield();
    }

cleanup:
    // remove ourselves from all events
    for (size_t i = 0; i < number_of_events; i++) {
        event_data_t* data = events[i];
        data->thread = NULL;
    }

    return err;
}

err_t close_event(event_t event) {
    err_t err = NO_ERROR;
    event_data_t* data = event;

    CHECK_ERROR(event != NULL, ERROR_INVALID_PARAM);

    aquire_event_lock();

    // check if in the dispatch list
    if (data->notify_link.next != NULL) {
        remove_entry_list(&data->notify_link);
    }

    release_event_lock();

    // free the event
    mm_free(data);

cleanup:
    return err;
}
