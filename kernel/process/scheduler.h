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

/**
 * Initialize the scheduler
 *
 * the scheduler will only start once interrupts are enabled
 */
void scheduler_init();

#endif
