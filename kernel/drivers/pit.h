#ifndef DRIVERS_PIT_H
#define DRIVERS_PIT_H

////////////////////////////////////////////////////////////////////////////
// Programmable Interrupt Timer
////////////////////////////////////////////////////////////////////////////
//
// This is a driver for using the pit, this will eventually support running_thread
// kernel callbacks as timer like stuff (interval/timeout), for right now it
// is just a simple way to control the pit controller.
//
// eventually this will need to be replaced with an APIC timer
//
////////////////////////////////////////////////////////////////////////////

#include <common/stdint.h>

/**
 * set the interval of the pit (in ms)
 */
void pit_set_interval(uint64_t interval);

/**
 * Get the interval of the pit (in ms)
 */
uint64_t pit_get_interval();

#endif
