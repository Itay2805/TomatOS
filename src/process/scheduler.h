#ifndef TOMATKERNEL_SCHEDULER_H
#define TOMATKERNEL_SCHEDULER_H

#include "thread.h"

extern thread_t* running_thread;

/**
 * Initialize the scheduler
 * @return
 */
error_t scheduler_init();

/**
 * Add a thread to the scheduler
 */
error_t scheduler_add(thread_t* thread);

#endif //TOMATKERNEL_SCHEDULER_H
