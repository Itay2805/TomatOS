#include <intr/apic/ioapic.h>
#include <intr/apic/lapic.h>
#include <intr/intr.h>

#include <event/event.h>

#include "pit.h"

#define TIMER_CONTROL_PORT 0x43
#define TIMER0_COUNT_PORT 0x40

static uint64_t period;

/**
 * Timer interrupt handler
 *
 * will simply send back an eoi and call the common timer handler
 */
static err_t interrupt_handler(void* ctx, interrupt_context_t* context) {
    send_apic_eoi();
    timer_tick(period);
    return NO_ERROR;
}

static interrupt_handler_t handler = {
    .name = "PIT8254 Interrupt",
    .vector = INT_ALLOCATE,
    .callback = interrupt_handler
};

static void set_pit_count(uint16_t count) {
    __outbyte(TIMER_CONTROL_PORT, 0x36);
    __outbyte(TIMER0_COUNT_PORT, count & 0xffu);
    __outbyte(TIMER0_COUNT_PORT, (count >> 8u) & 0xffu);
}

static err_t set_timer_period(uint64_t timer_period) {
    if (timer_period == 0) {
        ioapic_enable_interrupt(0, false);
    } else {
        // convert to ticks
        uint64_t timer_count = ((119318 * timer_period) + 500000) / 1000000;

        // handle overflow
        if (timer_count > 65536) {
            timer_count = 0;
            timer_period = 549254;
        }

        // program it
        set_pit_count(timer_count);

        // enable the interrupt
        ioapic_enable_interrupt(0, true);
    }

    period = timer_period;

    return NO_ERROR;
}

void init_pit8254() {
    // register the interrupts
    interrupt_register(&handler);
    ioapic_configure_interrupt(0, handler.vector, IO_APIC_DELIVERY_MODE_FIXED, false, true);
}

timer_t pit8254 = {
    .set_timer_period = set_timer_period,
};
