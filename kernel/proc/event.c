#include <mem/mm.h>
#include <util/stb_ds.h>
#include <sync/critical.h>
#include "event.h"
#include "scheduler.h"

typedef struct event_data {
    /**
     * The handle meta of the event
     */
    handle_meta_t handle_meta;

    /**
     * The event lock
     */
    ticket_lock_t lock;

    /**
     * Is the event signalled
     */
    atomic_bool signaled;

    /**
     * The waiting thread
     */
    thread_t* thread;

    /**
     * Do we have an activated timer
     */
    bool is_timer;

    /**
     * The trigger time of the timer
     */
    uint64_t trigger_time;

    /**
     * The period of the timer
     */
    uint64_t period;

    /**
     * Link to the timer list
     */
    list_entry_t link;
} event_data_t;

/**
 * List of all timers in the system, ordered by the time
 * to next timer
 */
static list_entry_t g_timers_list = INIT_LIST(g_timers_list);

/**
 * The lock on the timers list
 */
static ticket_lock_t g_timers_lock = INIT_LOCK();

/**
 * Will make sure to free the event
 */
static err_t free_event(handle_meta_t* meta) {
    event_data_t* data = CR(meta, event_data_t, handle_meta);

    // remove the timer
    if (data->is_timer) {
        ticket_lock(&g_timers_lock);
        list_del(&data->link);
        ticket_unlock(&g_timers_lock);
    }

    // free the event data
    kfree(data);

    return NO_ERROR;
}

err_t create_event(event_t* event) {
    err_t err = NO_ERROR;

    CHECK(event != NULL);

    event_data_t* new_event = kalloc(sizeof(event_data_t));
    new_event->handle_meta.kind = HANDLE_EVENT;
    new_event->handle_meta.refcount = 1;
    new_event->handle_meta.dtor = free_event;
    new_event->signaled = false;
    new_event->thread = NULL;
    new_event->lock = INIT_LOCK();

    *event = new_event;

cleanup:
    return err;
}

err_t signal_event(event_t event) {
    err_t err = NO_ERROR;
    event_data_t* data = event;
    critical_t crit;

    CHECK(event != NULL);

    enter_critical(&crit);
    ticket_lock(&data->lock);

    if (!data->signaled) {
        data->signaled = true;

        if (data->thread != NULL) {
            thread_t* thread = data->thread;
            data->thread = NULL;
            schedule_thread(thread);
        }
    }

cleanup:
    ticket_unlock(&data->lock);
    exit_critical(&crit);

    return err;
}

err_t check_event(event_t event) {
    err_t err = NO_ERROR;
    event_data_t* data = event;

    CHECK(event != NULL);

    bool yes = true;
    err = atomic_compare_exchange_weak(&data->signaled, &yes, false) ? NO_ERROR : ERROR_NOT_READY;

cleanup:
    return err;
}

err_t wait_for_event(event_t* events, size_t event_count, size_t* index) {
    err_t err = NO_ERROR;
    critical_t crit;

    CHECK(events != NULL);
    CHECK(event_count > 0);

    while (true) {
        // first lock everything
        enter_critical(&crit);
        for (int i = 0; i < event_count; i++) {
            event_data_t* data = events[i];
            ticket_lock(&data->lock);
        }

        // now we can safely iterate all the events and check them
        // one by one without anyone altering them in the middle
        for (int i = 0; i < event_count; i++) {
            event_data_t* data = events[i];

            err = check_event(data);
            if (err == NO_ERROR) {
                if (index != NULL) *index = i;
                goto cleanup;
            } else if (err == ERROR_NOT_READY) {

            } else {
                CHECK_AND_RETHROW(err);
            }

            CHECK_TRACE(data->thread == NULL || data->thread == g_current_thread, "Thread `%s` already listening on event (requester `%s`)", data->thread->name, g_current_thread->name);
            data->thread = g_current_thread;
        }

        g_current_thread->state = STATE_WAITING;

        // unlock everything before we yield
        for (int i = 0; i < event_count; i++) {
            event_data_t* data = events[i];
            ticket_unlock(&data->lock);
        }
        exit_critical(&crit);

        // yield
        yield();
    }

cleanup:
    if (events != NULL && event_count > 0) {
        // unregister waiting for all of these events
        // and unlock all the events
        for (int i = 0; i < event_count; i++) {
            event_data_t* data = events[i];

            if (data->thread == g_current_thread) {
                data->thread = NULL;
            }

            ticket_unlock(&data->lock);
        }
        exit_critical(&crit);
    }
    return err;
}

err_t close_event(event_t event) {
    err_t err = NO_ERROR;
    event_data_t* data = event;

    CHECK(event != NULL);

    CHECK_AND_RETHROW(release_handle_meta(&data->handle_meta));

cleanup:
    return err;
}


static void insert_event_timer(event_data_t* event) {
    ticket_lock(&g_timers_lock);

    list_entry_t* curr = NULL;
    FOR_EACH(curr, &g_timers_list) {
        event_data_t* data = CR(curr, event_data_t, link);
        if (data->trigger_time > event->trigger_time) {
            break;
        }
    }

    event->is_timer = true;
    list_add_tail(curr, &event->link);

    ticket_unlock(&g_timers_lock);
}

err_t set_timer(event_t timer, timer_type_t type, uint64_t trigger_time) {
    err_t err = NO_ERROR;
    event_data_t* data = timer;

    CHECK(timer != NULL);


    if (type == TIMER_CANCEL) {
        ticket_lock(&g_timers_lock);
        list_del(&data->link);
        ticket_unlock(&g_timers_lock);
        data->is_timer = false;
    } else {
        // set the period to the given time
        if (type == TIMER_PERIODIC) {
            data->period = trigger_time;
        }

        // set the trigger time and add it
        data->trigger_time = uptime() + trigger_time;
        insert_event_timer(data);
    }

cleanup:
    return err;
}

err_t process_timers() {
    err_t err = NO_ERROR;

    if (ticket_try_lock(&g_timers_lock)) {
        goto cleanup;
    }

    uint64_t system_time = uptime();

    while (!list_is_empty(&g_timers_list)) {
        event_data_t* te = CR(g_timers_list.next, event_data_t, link);

        // this timer is not expired yet, finish here
        if (te->trigger_time > system_time) {
            break;
        }

        // remove it
        list_del(&te->link);
        te->is_timer = false;

        // signal it
        WARN(!IS_ERROR(signal_event(te)), "Failed to signal timer event");

        // setup the timer again if periodic
        if (te->period != 0) {
            te->trigger_time = te->trigger_time + te->period;

            // if too much time has passed then reset it from this point
            if (te->trigger_time < system_time) {
                te->trigger_time = system_time;
            }

            // add it back
            te->is_timer = true;
            insert_event_timer(te);
        }
    }

    ticket_unlock(&g_timers_lock);

cleanup:
    return err;
}
