#include <event/timers/pit8254/pit.h>
#include <sync/spinlock.h>
#include <util/list.h>
#include <mm/mm.h>
#include "timer.h"

// the timer we are using
static timer_t* timer = NULL;

// system time
static atomic_intmax_t current_system_time = 0;

// timers event related
static event_t check_timers_event = NULL;
static list_entry_t timers_list = INIT_LIST_ENTRY(timers_list);
static spinlock_t timer_lock = SPINLOCK_INIT;

typedef struct event_timer {
    event_t event;
    uint64_t trigger_time;
    uint64_t period;
    list_entry_t link;
} event_timer_t;

/**
 * Will insert the given event timer in the correct place in the list
 */
static void insert_event_timer(event_timer_t* te) {
    list_entry_t* link = NULL;
    for (link = timers_list.next; link != &timers_list; link = link->next) {
        event_timer_t* te2 = CR(link, event_timer_t, link);
        if (te2->trigger_time > te->trigger_time) {
            break;
        }
    }

    insert_tail_list(&timers_list, &te->link);
}

/**
 * Will trigger any timers that need to be triggered
 */
static err_t check_timers(void* ctx, event_t event) {
    err_t err = NO_ERROR;

    spinlock_acquire(&timer_lock);
    uint64_t system_time = current_system_time;

    while (!is_list_empty(&timers_list)) {
        event_timer_t* te = CR(timers_list.next, event_timer_t, link);

        // this timer is not expired yet, finish here
        if (te->trigger_time > system_time) {
            break;
        }

        // remove it
        remove_entry_list(&te->link);
        te->link.next = NULL;

        // signal it
        WARN(!IS_ERROR(signal_event(te->event)), "Error signalling timer event");

        // setup the timer again if periodic
        if (te->period != 0) {
            te->trigger_time = te->trigger_time + te->period;

            // if too much time has passed then reset it from this point
            if (te->trigger_time < system_time) {
                te->trigger_time = system_time;
                signal_event(check_timers_event);
            }

            // add it back
//            insert_event_timer(te);
        } else {
            // free it
            mm_free(te);
        }
    }

    spinlock_release(&timer_lock);

cleanup:
    return err;
}

void init_timer() {
    err_t err = NO_ERROR;

    TRACE("timer: Initializing event timer");

    // choose the timer to use
    TRACE("timer: Using PIT8254 as timer");
    timer = &pit8254;
    init_pit8254();

    // setup the timers
    TRACE("timer: Configuring timer to 10ms");
    CHECK_AND_RETHROW(timer->set_timer_period(MS_TO_100NS(5)));
    CHECK_AND_RETHROW(create_event(TPL_TIMER, check_timers, NULL, &check_timers_event));

cleanup:
    ASSERT(!IS_ERROR(err));
}

void timer_tick(uint64_t passed) {
    current_system_time += passed;

    // check if need to check timers
    if (!is_list_empty(&timers_list)) {
        event_timer_t* te = CR(timers_list.next, event_timer_t, link);

        if (te->trigger_time <= current_system_time) {
            signal_event(check_timers_event);
        }
    }
}

uint64_t get_current_system_time() {
    return current_system_time / 10000;
}

err_t set_timer(event_t event, timer_type_t type, uint64_t trigger_time) {
    err_t err = NO_ERROR;

    CHECK_ERROR(event != NULL, ERROR_CHECK_FAILED);

    spinlock_acquire(&timer_lock);

    event_timer_t* new_te = NULL;

    // check if already in the timer queue
    FOR_EACH_IN_LIST(timers_list, link) {
        event_timer_t* te = CR(link, event_timer_t, link);
        if (te->event == event) {
            remove_entry_list(&te->link);
            new_te = te;
        }
    }

    // we wanna cancel it, free everything
    if (type == TIMER_CANCEL) {
        CHECK_ERROR(new_te != NULL, ERROR_NOT_FOUND);
        mm_free(new_te);

    } else {

        // if not found allocate one
        if (new_te == NULL) {
            new_te = mm_allocate(sizeof(event_timer_t));
        }

        // set the period to the given time
        if (type == TIMER_PERIODIC) {
            new_te->period = trigger_time;
        }

        // set the trigger time and add it
        new_te->trigger_time = current_system_time + trigger_time;
        new_te->event = event;
        insert_event_timer(new_te);
    }

cleanup:
    spinlock_release(&timer_lock);
    return err;
}
