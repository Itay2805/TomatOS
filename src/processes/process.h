#ifndef TOMATKERNEL_ROCESS_H
#define TOMATKERNEL_ROCESS_H

#include <memory/vmm.h>
#include "thread.h"

typedef size_t pid_t;

typedef struct process {
    // TODO: Move to a kobject
    int refcount;
    spinlock_t lock;

    // the process id
    pid_t pid;

    // The address space of the process
    address_space_t address_space;

    // the threads of the process
    struct {
        tid_t key;
        thread_t* value;
    }* threads;

    // the next tid to generate
    // starts from 0
    tid_t next_tid;

    // TODO: resources
    // TODO: default signal routing
} process_t;

typedef struct process_map_entry {
    pid_t key;
    process_t* value;
} process_map_entry_t;

/*
 * This is the kernel process
 */
extern process_t* kernel_process;
extern process_map_entry_t* processes;
extern spinlock_t processes_lock;

/////////////////////////////////////////////
// Kernel object management
/////////////////////////////////////////////

error_t create_process(address_space_t address_space, process_t** process);

error_t release_process(process_t* process);

/////////////////////////////////////////////
// Process API
/////////////////////////////////////////////

error_t process_kill(process_t* process, registers_t* regs);

#endif //TOMATKERNEL_ROCESS_H
