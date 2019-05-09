#ifndef PROCESS_SCHEDULER_H
#define PROCESS_SCHEDULER_H

////////////////////////////////////////////////////////////////////////////
// Scheduler
////////////////////////////////////////////////////////////////////////////
//
// This is the scheduler, it is in charge of choosing what thread to run
// each tick. Currently we only use a single core but hopefully in the
// future we will support SMP scheduling
//
////////////////////////////////////////////////////////////////////////////

#include <common/string.h>

#include <process/thread.h>

extern thread_t* running_thread;

/**
 * Initialize the scheduler
 *
 * the scheduler will only start once interrupts are enabled
 */
error_t scheduler_init();

void schedule(registers_t* new_state, int delta);

#endif
