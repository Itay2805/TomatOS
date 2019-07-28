#ifndef TOMATOS_SCHEDULER_H
#define TOMATOS_SCHEDULER_H

#include <error.h>
#include "thread.h"

error_t scheduler_init();

error_t scheduler_start_per_core();

error_t scheduler_kickstart();

/**
 * Tells the scheduler that the given thread is to be killed
 *
 * @param thread
 */
error_t scheduler_kill_thread(thread_t* thread);

#endif //TOMATOS_SCHEDULER_H
