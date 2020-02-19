#ifndef __PROC_PROCESS_H__
#define __PROC_PROCESS_H__

#include <sync/spinlock.h>
#include <mm/vmm.h>
#include <util/list.h>

struct thread;

typedef int pid_t;
typedef int tid_t;

typedef struct process {
    // the process id
    pid_t pid;

    // tid generation, starts from 0 including
    tid_t tid_gen;

    // the link to the process list
    list_entry_t link;

    // the handle to the vmm of the process
    vmm_handle_t vmm_handle;

    // the threads of the process, lock whenever changing stuff with it
    spinlock_t threads_lock;
    list_entry_t threads_list;
} process_t;

/**
 * List of all the process in the system
 */
extern spinlock_t process_lock;
extern list_entry_t process_list;

/**
 * The kernel process
 */
extern process_t kernel_process;

/**
 * Initialize the kernel process
 */
void init_kernel_process();

#endif //__PROC_PROCESS_H__
