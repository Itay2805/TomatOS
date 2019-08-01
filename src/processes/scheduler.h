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
 * Tells the scheduler that the given thread is to be killed
 *
 * @param thread
 */
error_t scheduler_kill_thread(thread_t* thread);

#endif //TOMATOS_SCHEDULER_H
