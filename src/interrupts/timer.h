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

#endif //TOMATKERNEL_TIMER_H
