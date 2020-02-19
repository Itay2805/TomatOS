#ifndef __EVENT_TIMERS_TIMER_H__
#define __EVENT_TIMERS_TIMER_H__

#include <util/except.h>
#include <stdint.h>

typedef struct timer {
    err_t (*set_timer_period)(uint64_t timer_period);
} timer_t;

/**
 * Initialize timers
 */
void init_timer();

/**
 * This is the global timer tick function
 *
 * It is supposed to be called from the different timer handlers
 *
 * @param passed    [IN] The time passed in 100ns since the last tick
 */
void timer_tick(uint64_t passed);

/**
 * Returns the system time since timer initialization
 * in milliseconds
 */
uint64_t get_current_system_time();

#endif //__EVENT_TIMERS_TIMER_H__
