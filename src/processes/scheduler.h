#ifndef TOMATKERNEL_CHEDULER_H
#define TOMATKERNEL_CHEDULER_H

#include <error.h>
#include "thread.h"

/**
 * While accessing the running threads you MUST lock this lock
 */
spinlock_t running_threads_lock;

/**
 * An array of the running threads
 */
thread_t** running_threads;

/**
 * This will do the first initialization of the scheduler like creating the
 * idle threads.
 *
 * @remark
 * This will not start the scheduler itself, and can technically run before SMP is done.
 */
error_t scheduler_init();

/**
 * Will iterate all the processors and start the scheduler on each one
 * @return
 */
error_t scheduler_kickstart();

/**
 * Queue another thread into the scheduler
 */
error_t scheduler_queue_thread(thread_t* thread);

/**
 * Reschedule another thread
 *
 * @param regs  [IN/OUT] The cpu context
 */
error_t scheduler_reschedule(registers_t* regs);

/**
 * Tells the scheduler to remove a thread from the scheduler
 *
 * @remark
 * This function will not change the state of the thread!
 */
error_t scheduler_remove_thread(thread_t* thread, registers_t* regs);

#endif //TOMATKERNEL_CHEDULER_H
