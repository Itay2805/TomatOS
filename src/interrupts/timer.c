#include <buf.h>
#include <drivers/apic/lapic.h>
#include "timer.h"
#include "idt.h"

typedef struct timer {
    int64_t initial_millis;
    int64_t millis_left;
    interrupt_handler_f handler;
} timer_t;

static timer_t* timers = NULL;

void timer_interrupt_handler(registers_t regs) {
    error_t err = NO_ERROR;

    for(timer_t* timer = timers; timer < buf_end(timers); timer++) {
        timer->millis_left -= 10;
        if(timer->millis_left <= 0) {
            timer->millis_left = timer->initial_millis;
            CHECK_AND_RETHROW(timer->handler(&regs));
        }
    }

    CHECK_AND_RETHROW(lapic_send_eoi());

cleanup:
    if(err != NO_ERROR) {
        log_critical("Error in timer interrupt handler, halting :(");
        _cli();
        while(true) {
            _hlt();
        }
    }
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
    idt_set_entry(INTERRUPT_TIMER, timer_interrupt_stub, IDT_INTERRUPT_GATE, 0);
    return NO_ERROR;
}
