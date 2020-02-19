#ifndef __PROC_THREAD_H__
#define __PROC_THREAD_H__

#include <intr/intr.h>
#include "process.h"

typedef enum thread_state {
    // running right now
    STATE_RUNNING,

    // normal, not running but is scheduled
    STATE_NORMAL,

    // the thread is waiting for an event
    STATE_WAITING,

    // the thread is dead and should be freed when possible
    STATE_DEAD,
} thread_state_t;

typedef struct thread {
    // the thread id
    tid_t tid;

    // the parent of the thread
    process_t* parent;

    // the current thread state
    _Atomic(thread_state_t) state;

    // this is the stack used for the kernel
    // when doing syscalls, ignored for kernel
    // threads
    uintptr_t kernel_stack;

    // the context of the thread
    interrupt_context_t cpu_context;
    // TODO: gpu context

    // the link to the process thread list
    list_entry_t thread_link;

    // the link in the scheduler queue
    list_entry_t scheduler_link;

    // the TPL of the thread
    _Atomic(tpl_t) thread_tpl;
} thread_t;

/**
 * Spawn a new thread
 *
 * NOTE: If the new_thread pointer is provided then the thread will not be scheduled
 *       otherwise the function will queue the thread on its own
 *
 * @param parent        [IN]            The parent of the thread
 * @param rip           [IN]            The start of the new thread
 * @param stack         [IN]            The stack for the new thread
 * @param new_thread    [OUT,OPTIONAL]  The newly created thread
 */
err_t spawn_thread(process_t* parent, uintptr_t rip, uintptr_t stack, thread_t** new_thread);

#endif //__PROC_THREAD_H__
