#ifndef __PROC_PROCESS_H__
#define __PROC_PROCESS_H__

#include <compo/component.h>
#include <sync/spinlock.h>
#include <compo/fs/fs.h>
#include <util/list.h>
#include <mm/vmm.h>

#include "syscall.h"
#include "handle.h"

struct thread;

typedef int pid_t;
typedef int tid_t;

typedef struct process {
    // the process id
    pid_t pid;

    // reference count for the process
    atomic_int refcount;
    // TODO: Do I also want a process lock?

    // tid generation, starts from 0 including
    tid_t tid_gen;

    // the link to the process list
    list_entry_t link;

    // the handle to the vmm of the process
    vmm_handle_t vmm_handle;

    // the threads of the process, lock whenever changing stuff with it
    spinlock_t threads_lock;
    list_entry_t threads_list;

    // all the handles, only relevant to user processes
    spinlock_t handles_lock;
    int next_handle;
    struct {
        int key;
        handle_t value;
    }* handles;
} process_t;

/**
 * List of all the process in the system
 */
extern spinlock_t m_process_list_lock;
extern list_entry_t m_process_list;

/**
 * The kernel process
 */
extern process_t kernel_process;

/**
 * Initialize the kernel process
 */
void init_kernel_process();

/**
 * Will return a process by it's pid
 *
 * TODO: we probably want some reference counting on that
 *
 * @param pid       [IN]    The pid of the process
 * @param process   [OUT]   The process
 */
err_t get_process_by_pid(int pid, process_t** process);

/**
 * Create a new empty process
 *
 * @remark
 * The only thing that will happen is that an address space will be created for the process,
 * nothing else is gonna be initialized
 */
err_t create_process(process_t** process);

/**
 * Will spawn a new process from the given file handle
 *
 * @param file  [IN]    The file to start the process from
 * @param proc  [OUT]   The new process
 */
err_t spawn_process(file_t file, process_t** proc);

/**
 * Release the handle to the process
 *
 * if the ref count reaches zero it will kill the process
 *
 * @param process   [IN] The process to release the handle of
 */
err_t release_process(process_t* process);

/**
 * Will add an handle to the process
 *
 * @remark
 * This will increase the reference count
 *
 * @param process       [IN]    The process to add the handle to
 * @param handle        [IN]    The handle to add
 * @param out_handle    [OUT]   The user handle value
 */
err_t add_handle(process_t* process, handle_t handle, int* out_handle);

/**
 * Remove a handle from the process
 *
 * @param process   [IN] The process to remove the handle from
 * @param handle    [IN] The handle to remove
 */
err_t remove_handle(process_t* process, int handle);

/**
 * Get a handle from value
 *
 * @remark
 * You must call `close_handle` after using this handle since this will
 * increase the reference count of the handle
 *
 * @param process       [IN] The process the handle belond to
 * @param handle        [IN] The handle to get
 * @param out_handle    [IN] The kernel handle
 */
err_t get_handle(process_t* process, int handle, handle_t* out_handle);

/**
 * Copy memory to another process
 *
 * @remark
 * Given pointer must point to a kernel address!
 *
 * @param process   [IN]    The process to copy to
 * @param to        [IN]    The address to copy to
 * @param from      [IN]    The kernel address
 * @param size      [IN]    The size to copy
 */
err_t copy_to_process(process_t* process, void* to, void* from, size_t size);

#endif //__PROC_PROCESS_H__
