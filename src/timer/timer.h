#ifndef TOMATKERNEL_TIMER_H
#define TOMATKERNEL_TIMER_H

#include <error.h>
#include <interrupts/interrupts.h>

typedef uint64_t timer_t;

typedef error_t (*timer_handler_f)(registers_t* regs, timer_t timer);

/**
 * Will set up the interrupt timer, this is a high level initialization
 * which will call the specific driver initialization
 */
error_t timer_init();

/**
 * Will add an interval timer
 *
 * @param handler   [IN] The handler to call
 * @param millis    [IN] The time in millis
 * @param timer     [OUT]   An ID which can be used to cancel a timer
 */
error_t timer_register_interval(timer_handler_f handler, uint64_t millis, timer_t* timer);

/**
 * Will add a one time timer
 *
 * @param handler   [IN]    The handler to call
 * @param millis    [IN]    The time in millis
 * @param timer     [OUT]   An ID which can be used to cancel a timer
 */
error_t timer_register_timeout(timer_handler_f handler, uint64_t millis, timer_t* timer);

/**
 * Will cancel the given timer
 *
 * @param timer     [IN]    The timer to cancel
 */
error_t timer_cancel(timer_t timer);

/**
 * Will update the counters of the timers
 *
 * @remark
 * This function should only be used by timer drivers!
 *
 * @param millis    [IN] Time passed
 */
error_t timer_update(registers_t* regs, uint64_t millis);

#endif //TOMATKERNEL_TIMER_H
