#ifndef __TOMATOS_KERNEL_PROC_PROCESS_H__
#define __TOMATOS_KERNEL_PROC_PROCESS_H__

#include <arch/ints.h>
#include <util/list.h>
#include <arch/cpu.h>
#include <mem/vmm.h>
#include "handle.h"

struct thread;

/**
 * This represents a process
 */
typedef struct process {
    /**
     * The process id
     */
    int pid;

    /**
     * The name of the process
     */
    char name[32];

    /**
     * The meta of the process handle
     */
    handle_meta_t handle_meta;

    /**
     * The address space of the handle
     */
    address_space_t address_space;

    /**
     * Lock on the handles list
     * TODO: convert to rwlock
     */
    ticket_lock_t handles_lock;

    /**
     * The handles of the process
     */
    struct {
        handle_t key;
        handle_meta_t* value;
    }* handles;

    /**
     * used for generation handle ids
     */
     _Atomic(handle_t) next_handle;

     /**
      * The threads of this process
      */
     list_entry_t threads;

     /**
      * Lock for the thread list
      */
     ticket_lock_t threads_lock;

     /**
      * Next tid
      */
     atomic_int next_tid;

     /**
      * The static priority of the
      * process:
      *     - 0-99: real time
      *     - 100-139: normal
      */
     size_t priority;
} process_t;

typedef enum thread_state {
    /**
     * The thread is currently running
     *
     * @remark
     * This does not mean code is executing right now, it just
     * means that it will get scheduled
     */
    STATE_RUNNING,

    /**
     * The thread is waiting on something, if the scheduler sees
     * this state on the current thread, it dequeues the thread.
     */
    STATE_WAITING,

    /**
     * A dead thread, will not run and the context won't be saved
     */
    STATE_DEAD,
} thread_state_t;

/**
 * This represents a thread
 */
typedef struct thread {
    /**
     * The thread id
     */
    int tid;

    /**
     * The state of the process
     */
    _Atomic(thread_state_t) state;

    /**
     * The thread handle meta
     */
    handle_meta_t handle_meta;

    /**
     * The parent process
     */
    process_t* parent;

    /**
     * The link in the process list
     */
    list_entry_t link;

    /**
     * The name of the thread
     */
    char name[32];

    /**
     * The execution context
     */
    system_context_t system_context;

    /**
     * The stack of the thread
     */
    void* stack;

    ///////////////////////////////////
    // Scheduling vars
    ///////////////////////////////////

    /**
     * the link in the scheduler
     */
    list_entry_t scheduler_link;

    /**
     * The last cpu
     */
    int last_cpu;

    /**
     * The last time the thread was ran
     */
    uint64_t last_run;

    /**
     * The time the thread spent on sleeping
     */
    uint64_t sleep_time;

    /**
     * The dynamic priority of this thread
     */
    size_t priority;
} thread_t;

/**
 * The kernel process
 */
extern process_t g_kernel;

/**
 * Pointer to the current process
 */
extern process_t* CPU_LOCAL g_current_process;

/**
 * Pointer to the current thread
 */
extern thread_t* CPU_LOCAL g_current_thread;

/**
 * Will initialize the ripper thread
 */
err_t init_ripper();

/**
 * Create a thread for the current process
 *
 * @param thread    [OUT]   The new thread
 * @param func      [IN]    The entry function
 * @param data      [IN]    The data to pass to the entry
 * @param name      [IN]    The name of the thread, can be null
 */
err_t create_thread(thread_t** thread, void(*func)(void* data), void* data, const char* name);

/**
 * Will exit from the current thread
 */
err_t exit();

/**
 * Will close the thread
 *
 * @remark
 * This will not actually kill the thread.
 *
 * @remark
 * Internally this will simply call the release_handle_meta
 * on the threads's handle_meta
 */
err_t release_thread(thread_t* thread);

#endif //__TOMATOS_KERNEL_PROC_PROCESS_H__
