#ifndef TOMATKERNEL_TIMER_H
#define TOMATKERNEL_TIMER_H

#include <stdint.h>
#include <error.h>
#include "interrupts.h"

/**
 * Will set up the interrupt timer, anything which wants to run every
 * X millis will go in here
 *
 * @remark
 * The resolution of this is about ~10 millis
 */
error_t timer_init();

/**
 * The timer interrupt stub, calls the timer_interrupt_handler
 */
extern void timer_interrupt_stub();

/**
 * The timer interrupt handler, called from the stub
 */
void timer_interrupt_handler(registers_t regs);

#endif //TOMATKERNEL_TIMER_H
