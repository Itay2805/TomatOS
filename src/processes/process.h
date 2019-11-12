#ifndef TOMATKERNEL_PROCESS_H
#define TOMATKERNEL_PROCESS_H

#include <objects/object.h>
#include <util/list.h>
#include <memory/vmm.h>
#include "thread.h"

typedef struct process {
    object_t _;

    lock_t lock;
    vmm_handle_t vmm_handle;
    thread_t** threads;
} process_t;

extern process_t* kernel_process;

/**
 * Get the process cast
 */
const void* Process();

#endif //TOMATKERNEL_PROCESS_H
