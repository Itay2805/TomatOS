#include <mem/mm.h>
#include <util/stb_ds.h>
#include "event.h"
#include "scheduler.h"

typedef struct event_data {
    /**
     * The handle meta of the event
     */
    handle_meta_t handle_meta;

    /**
     * Is the event signalled
     */
    atomic_bool signaled;

    /**
     * The waiting thread
     */
    thread_t* thread;
//
//    /**
//     * Do we have an activated timer
//     */
//    bool is_timer;
//
//    /**
//     * The trigger time of the timer
//     */
//    uint64_t trigger_time;
//
//    /**
//     * The period of the timer
//     */
//    uint64_t period;
//
//    /**
//     * Link to the timer list
//     */
//    list_entry_t link;
} event_data_t;

static err_t free_event(handle_meta_t* meta) {
    event_data_t* data = CR(meta, event_data_t, handle_meta);
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

    *event = new_event;

cleanup:
    return err;
}

err_t signal_event(event_t event) {
    err_t err = NO_ERROR;
    event_data_t* data = event;

    CHECK(event != NULL);

    if (!data->signaled) {
        data->signaled = true;

        if (data->thread != NULL) {
            thread_t* thread = data->thread;
            data->thread = NULL;
            schedule_thread(thread);
        }
    }

cleanup:
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

    CHECK(events != NULL);
    CHECK(event_count > 0);

    while (true) {
        for (int i = 0; i < event_count; i++) {
            err = check_event(events[i]);
            if (err == NO_ERROR) {
                if (index != NULL) *index = i;
                goto cleanup;
            } else if (err == ERROR_NOT_READY) {

            } else {
                CHECK_AND_RETHROW(err);
            }

            event_data_t* data = events[i];
            CHECK(data->thread == NULL);
            data->thread = g_current_thread;
        }

        g_current_thread->state = STATE_WAITING;
        yield();
    }

cleanup:
    if (events != NULL) {
        for (int i = 0; i < event_count; i++) {
            event_data_t* data = events[i];

            if (data->thread == g_current_thread) {
                data->thread = NULL;
            }
        }
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
