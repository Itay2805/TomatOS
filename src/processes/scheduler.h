#ifndef TOMATOS_SCHEDULER_H
#define TOMATOS_SCHEDULER_H

#include <error.h>
#include "thread.h"

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
 * Tells the scheduler to remove a thread from the scheduler
 *
 * @remark
 * This will not handle running threads!
 */
error_t scheduler_remove_thread(thread_t* thread);

#endif //TOMATOS_SCHEDULER_H
