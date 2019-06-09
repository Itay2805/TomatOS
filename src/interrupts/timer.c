#include <buf.h>
#include "timer.h"
#include "idt.h"
#include "irq.h"

typedef struct timer {
    int64_t initial_millis;
    int64_t millis_left;
    interrupt_handler_f handler;
} timer_t;

static timer_t* timers = NULL;

error_t timer_interrupt_handler(registers_t* regs) {
    error_t err = NO_ERROR;

    for(timer_t* timer = timers; timer < buf_end(timers); timer++) {
        timer->millis_left -= 1;
        if(timer->millis_left <= 0) {
            timer->millis_left = timer->initial_millis;
            CHECK_AND_RETHROW(timer->handler(regs));
        }
    }

cleanup:
    return err;
}

error_t timer_add(interrupt_handler_f handler, uint64_t time) {
    buf_push(timers, (timer_t){
        .initial_millis = time,
        .millis_left = 0,
        .handler = handler
    });
    return NO_ERROR;
}

error_t timer_init() {
    log_info("Setting timer");
    irq_set_handler(INTERRUPT_TIMER, timer_interrupt_handler);
    return NO_ERROR;
}
