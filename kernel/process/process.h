#ifndef PROCESS_H
#define PROCESS_H

#include <common/map.h>
#include <memory/vmm.h>
#include <memory/mm.h>
#include "thread.h"

#define DEAD_PROCESS_PID 0

/*
 * TODO:    the process should be attached with a loader, which will handle the loading and unloading \
 *          of the process from a resource, it will be called on the thread creation and destruction as
 *          well as the process creation and destruction. it will mainly manange the memory mapping of the
 *          process.
 */

/**
 * Represents a process context
 */
struct process {
    /**
     * The process PID
     */
    size_t pid;

    /**
     * The next thread tid for that process
     */
    size_t next_tid;

    /**
     * The thread list of this process (this is a stretchy buffer)
     */
    thread_t* threads;

    /**
     * The process address space
     */
    address_space_t address_space;

    /**
     * Is this a kernel process
     */
    bool kernel;
};

typedef struct process process_t;

/**
 * The process list
 *
 * contains all the processes
 */
extern process_t* processes;

/**
 * Create a new process
 * if the process is running as kernel it will allow it to run code in the kernel.
 *
 * this also creates the main thread
 */
process_t* process_create(thread_start_f start, bool kernel);

/**
 * find a process by it's PID
 */
process_t* process_find(size_t pid);

/**
 * Create a new thread for the process
 *
 * use thread_* functions to continue doing stuff with the thread
 */
thread_t* process_start_thread(process_t* process, thread_start_f start);

/**
 * Will remove the process and all of it's resources
 * this happens immedietly
 *
 * TODO:    figure out how signals should be handled
 *          so we can request a process to just exit
 */
void process_remove(process_t* process);

#endif
