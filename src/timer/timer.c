#include <stb/stb_ds.h>
#include <common/locks/spinlock.h>
#include <timer/hpet/hpet.h>
#include "timer.h"

typedef struct timer_entry {
    timer_handler_f handler;
    uint64_t interval;
    int64_t left;
} timer_entry_t;

static struct {
    timer_t key;
    timer_entry_t value;
}* timers;

static timer_t id_gen = 1;

static spinlock_t timer_spinlock;

// TODO: Maybe we want the timers to not be handled that much by the timer abstraction
// TODO: but more by the actual driver to allow for better optimizations and more accuracy

error_t timer_init() {
    error_t err = NO_ERROR;

    if(hpet_is_supported()) {
        CHECK_AND_RETHROW(hpet_init());
    }else {
        CHECK_FAIL_ERROR_TRACE(ERROR_NOT_FOUND, "No timer driver is supported for this hardware");
    }

cleanup:
    return err;
}

error_t timer_register_interval(timer_handler_f handler, uint64_t millis, timer_t* timer) {
    error_t err = NO_ERROR;

    CHECK(handler);
    CHECK(millis > 0);

    // setup
    lock_preemption(&timer_spinlock);

    timer_t id = id_gen++;
    timer_entry_t entry = {
            .handler = handler,
            .interval = millis,
            .left = millis
    };

    // add it
    hmput(timers, id, entry);

    *timer = id;

cleanup:
    unlock_preemption(&timer_spinlock);
    return err;
}

error_t timer_register_timeout(timer_handler_f handler, uint64_t millis, timer_t* timer) {
    error_t err = NO_ERROR;

    CHECK(handler);
    CHECK(millis > 0);

    // setup
    lock_preemption(&timer_spinlock);

    timer_t id = id_gen++;
    timer_entry_t entry = {
            .handler = handler,
            .interval = 0,
            .left = millis
    };

    // add it
    hmput(timers, id, entry);

    *timer = id;

cleanup:
    unlock_preemption(&timer_spinlock);
    return err;
}

error_t timer_cancel(timer_t timer) {
    lock_preemption(&timer_spinlock);
    hmdel(timers, timer);
    unlock_preemption(&timer_spinlock);
    return NO_ERROR;
}

error_t timer_update(registers_t* regs, uint64_t millis) {
    lock_preemption(&timer_spinlock);
    timer_t* to_remove = NULL;

    // iterate and update the times
    // if needed call the handler
    for(int i = 0; i < hmlen(timers); i++) {
        timers[i].value.left -= millis;
        if(timers[i].value.left <= 0) {
            if(timers[i].value.interval != 0) {
                timers[i].value.left = timers[i].value.interval;
            }else {
                // this is none-interval, add to an array of removes
                arrpush(to_remove, timers[i].key);
            }
            timers[i].value.handler(regs, timers[i].key);
        }
    }

    // remove all the timers
    for(int i = 0; i < arrlen(to_remove); i++) {
        hmdel(timers, to_remove[i]);
    }
    arrfree(to_remove);

    // unlock
    unlock_preemption(&timer_spinlock);
    return NO_ERROR;
}
