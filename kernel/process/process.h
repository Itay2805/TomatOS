#ifndef PROCESS_H
#define PROCESS_H

#include <common/map.h>
#include <memory/vmm.h>
#include "thread.h"

#define DEAD_PROCESS_PID 0

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
     * Memory manager context, for dynamic memory management
     */
    mm_context_t mm_context;
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
 *
 * this also creates the main thread
 */
process_t* process_create(thread_start_f start);

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
