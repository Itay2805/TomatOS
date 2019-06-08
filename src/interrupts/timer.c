#include <buf.h>
#include <drivers/apic/lapic.h>
#include "timer.h"
#include "idt.h"

typedef struct timer {
    size_t initial_millis;
    size_t millis_left;
    interrupt_handler_f handler;
} timer_t;

static timer_t* timers;

static void interrupt_handler(registers_t regs) {
    error_t err = NO_ERROR;

    for(timer_t* timer = timers; timer < buf_end(timers); timer++) {
        timer->millis_left -= 10;
        if(timer->millis_left <= 0) {
            timer->millis_left = timer->initial_millis;
            CHECK_AND_RETHROW(timer->handler(&regs));
        }
    }
    log_info("Hello there");

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

error_t timer_init() {
    log_info("Setting timer");
    idt_set_entry(INTERRUPT_TIMER, interrupt_handler, IDT_INTERRUPT_GATE, 0);
    return NO_ERROR;
}
