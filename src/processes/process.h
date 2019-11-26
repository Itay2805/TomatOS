#ifndef TOMATKERNEL_PROCESS_H
#define TOMATKERNEL_PROCESS_H

#include <util/list.h>
#include <memory/vmm.h>
#include "thread.h"

typedef struct process {
    lock_t lock;
    vmm_handle_t vmm_handle;
    thread_t** threads; // TODO: Turn into a hashmap?
} process_t;

/**
 * This is the kernel process
 */
extern process_t* kernel_process;

/**
 * Create a new process
 */
process_t* new_process();

#endif //TOMATKERNEL_PROCESS_H
