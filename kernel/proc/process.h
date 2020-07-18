#ifndef __TOMATOS_KERNEL_PROC_PROCESS_H__
#define __TOMATOS_KERNEL_PROC_PROCESS_H__

#include <mem/vmm.h>
#include <arch/cpu.h>
#include <util/list.h>
#include "handle.h"

struct thread;

/**
 * This represents a process
 */
typedef struct process {
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
     handle_t next_handle;

     /**
      * The threads of this process
      */
     list_entry_t threads;

     /**
      * The static priority of the
      * process:
      *     - 0-99: real time
      *     - 100-139: normal
      */
     size_t priority;
} process_t;

/**
 * This represents a thread
 */
typedef struct thread {
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

    ///////////////////////////////////
    // Scheduling vars
    ///////////////////////////////////

    /**
     * the link in the scheduler
     */
    list_entry_t scheduler_link;

    /**
     * The dynamic priority of this thread
     */
    size_t priority;

    /**
     *
     */
    size_t last_run;
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
 * This will initialize the kernel process
 */
err_t init_kernel_process();

#endif //__TOMATOS_KERNEL_PROC_PROCESS_H__
