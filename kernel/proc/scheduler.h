#ifndef __TOMATOS_KERNEL_PROC_SCHEDULER_H__
#define __TOMATOS_KERNEL_PROC_SCHEDULER_H__

#include <util/except.h>
#include <arch/ints.h>
#include "process.h"

void init_scheduler(size_t cpu_count);

/**
 * Add a new process for scheduling
 *
 * @param process   [IN]
 */
err_t schedule_thread(thread_t* process);

/**
 *
 * @param process
 * @return
 */
err_t deschedule_thread(thread_t* process);

/**
 * Start the scheduler on the current cpu
 */
void startup_scheduler();

#endif //__TOMATOS_KERNEL_PROC_SCHEDULER_H__
