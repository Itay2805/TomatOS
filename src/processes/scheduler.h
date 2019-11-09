#ifndef TOMATKERNEL_SCHEDULER_H
#define TOMATKERNEL_SCHEDULER_H

#include "thread.h"

/**
 * This will initialize the scheduler for the current thread
 *
 * including setting the
 */
void per_cpu_scheduler_init();

/**
 * Will queue a thread in the scheduler
 */
void scheduler_queue_thread(thread_t* thread);

#endif //TOMATKERNEL_SCHEDULER_H
