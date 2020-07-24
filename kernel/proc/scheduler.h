#ifndef __TOMATOS_KERNEL_PROC_SCHEDULER_H__
#define __TOMATOS_KERNEL_PROC_SCHEDULER_H__

#include <util/except.h>
#include <arch/ints.h>
#include "process.h"

/**
 * A per-cpu stack used for idle tasks
 */
extern uintptr_t CPU_LOCAL g_idle_stack;

void init_scheduler(size_t cpu_count);

/**
 * Add a new process for scheduling
 *
 * @param process   [IN]
 */
void schedule_thread(thread_t* process);

/**
 * Start the scheduler on the current cpu
 */
void startup_scheduler();

/**
 * Will yield to the scheduler
 */
void yield();

#endif //__TOMATOS_KERNEL_PROC_SCHEDULER_H__
