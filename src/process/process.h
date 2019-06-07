#ifndef TOMATKERNEL_PROCESS_H
#define TOMATKERNEL_PROCESS_H

#include <map.h>
#include <memory/vmm.h>
#include "thread.h"

typedef struct process {
    // the process pid
    int pid;

    // this process was
    bool canceled;

    // the address space of the process
    address_space_t address_space;

    // the next tid to give to the process
    int next_tid;

    // holds a list of all threads
    map_t threads;
} process_t;

/**
 * map<int, process_t*>
 */
extern map_t processes;

// TODO: error_t process_spawn(process_t** process, );

/**
 * Create a kernel process, which will run in the kernel
 */
error_t process_create(process_t** process, void*(start_routine)(void*), int argc, const char* argv[]);

/**
 * Will cancel the process
 */
error_t process_cancel(process_t* process);

/**
 * Validate a process is still valid
 */
bool process_validate(process_t* process);

#endif //TOMATKERNEL_PROCESS_H
