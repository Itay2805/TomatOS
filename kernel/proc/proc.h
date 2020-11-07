#ifndef TOMATOS_PROC_H
#define TOMATOS_PROC_H

#include <mem/vmm.h>
#include <cont/list.h>
#include <sync/lock.h>
#include "handle.h"

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

    // TODO: handles stuff

    // TODO: thread stuff
} process_t;

// TODO: thread stuff

/**
 * The kernel process
 */
extern process_t g_kernel;

#endif //TOMATOS_PROC_H
