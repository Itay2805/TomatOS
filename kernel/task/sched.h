#ifndef __TOMATOS_SCHED_H__
#define __TOMATOS_SCHED_H__

#include <util/except.h>
#include <stdnoreturn.h>

/**
 * Queue a new task, this will also wakeup all the idle cpus
 * so they can try and take the task
 *
 * @param task  [IN] The task to queue
 */
err_t queue_task(task_t* task);

/**
 * This starts a loop of dispatching available tasks, idling
 * if there is nothing else to do
 */
noreturn void task_dispatcher();

#endif //__TOMATOS_SCHED_H__
