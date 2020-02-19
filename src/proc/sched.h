#ifndef __PROC_SCHED_H__
#define __PROC_SCHED_H__

#include <smp/pcpu.h>
#include "thread.h"

/**
 * The currently running thread on this cpu
 */
extern thread_t* CPU_LOCAL current_thread;

/**
 * Initialize the scheduler
 *
 * just setup the yield interrupts
 */
err_t init_sched();

/**
 * Queue a thread to run
 */
err_t queue_thread(thread_t* thread);

/**
 * Run the scheduler, will reschedule the currently executing task
 *
 * @param ctx   [IN] The interrupt context to modify
 */
err_t sched_tick(interrupt_context_t* ctx);

/**
 * Will fire a yield interrupt, rescheduling the thread
 */
void yield();

#endif //__SRC_PROC_SCHED_H__
