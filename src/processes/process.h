#ifndef TOMATKERNEL_PROCESS_H
#define TOMATKERNEL_PROCESS_H

#include <util/list.h>
#include <objects/objects.h>

typedef struct process {
    object_t object;

    // Process lock
    // TODO read write
    lock_t lock;

    // the virtual space
    vmm_handle_t vmm_handle;

    // list of the threads in the process
    list_entry_t threads;
} process_t;

/*
 * The processes list
 */
list_entry_t processes;

/*
 * Represents the kernel process, all the kernel
 * threads live under it and it should never die
 */
process_t kernel_process;

/**
 * Will initialize the kernel process
 */
void kernel_process_init();

/**
 * Initialize a process with default values
 *
 * @remark
 * This will also add the process to the processes list
 *
 * @param process   [IN/OUT] The process to initialize
 */
void process_init(process_t* process);

#endif //TOMATKERNEL_PROCESS_H
