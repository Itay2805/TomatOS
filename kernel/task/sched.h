#ifndef TOMATOS_SCHED_H
#define TOMATOS_SCHED_H

#include "task.h"

/**
 * Will cause the task to be scheduled on the cpu with the least
 * amount of current tasks
 */
#define ANY_CPU (-1)

/**
 * TODO: this
 */
void init_sched(int cpus);

/**
 * TODO: this
 */
void queue_task(int cpu, task_t* task);

/**
 * Will start scheduling on the current cpu
 */
void start_schedule_loop();

#endif //TOMATOS_SCHED_H
