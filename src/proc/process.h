#ifndef __PROC_PROCESS_H__
#define __PROC_PROCESS_H__

#include <sync/spinlock.h>
#include <mm/vmm.h>
#include <util/list.h>
#include <compo/component.h>
#include <compo/fs/fs.h>

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

/**
 * Create a new empty process
 *
 * @remark
 * The only thing that will happen is that an address space will be created for the process,
 * nothing else is gonna be initialized
 */
err_t create_process(process_t** process);

/**
 * Spawn a new process from the given file at the given filesystem
 */
err_t spawn_process(file_t file, process_t** proc);

#endif //__PROC_PROCESS_H__
