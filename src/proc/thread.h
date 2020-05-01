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
    _Atomic(uintptr_t) apic_id;

    // this is the stack used for the kernel
    // when doing syscalls, ignored for kernel
    // threads
    uintptr_t kernel_stack;
    uintptr_t saved_stack;

    // the context of the thread
    interrupt_context_t cpu_context;
    void* simd_state;

    // the link to the process thread list
    list_entry_t thread_link;

    // the link in the scheduler queue
    list_entry_t scheduler_link;

    // the TPL of the thread
    _Atomic(tpl_t) thread_tpl;

    // context if the current syscall (if any)
    syscall_context_t* syscall_ctx;
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

/**
 * Restore the thread global context so it can
 * continue run correctly
 *
 * @param thread    [IN] The thread to restore
 * @param ctx       [IN] The context of the schedule interrupt
 */
void restore_thread_context(thread_t* thread, interrupt_context_t* ctx);

/**
 * Save the thread global state so it can continue run later
 *
 * @param thread    [IN] The thread to restore
 * @param ctx       [IN] The context of the schedule interrupt
 */
void save_thread_context(thread_t* thread, interrupt_context_t* ctx);

/**
 * Handle a user exception properly
 */
void on_user_exception(interrupt_context_t* ctx);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// syscall stuff
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// start a thread in a STATE_WAITING state
#define THRD_SPAWN_WAIT (1 << 0)

err_t sys_spawn_thread(syscall_context_t* ctx);

// set the thread state
#define THRD_OPT_STATE  (0)
#define THRD_STATE_NORMAL (0)
#define THRD_STATE_WAITING (1)

err_t sys_thrd_set_opt(syscall_context_t* ctx);

#endif //__PROC_THREAD_H__
