#ifndef PROCESS_THREAD_H
#define PROCESS_THREAD_H

#include <common/stdint.h>
#include <interrupts/interrupts.h>

// forward declare
struct process;

/**
 * The different thread states
 */
typedef enum thread_state {
    /**
     * The thread is ready to run, but something else is running now
     */
    THREAD_NORMAL,

    /**
     * The thread is now running
     */
    THREAD_RUNNING,

    /**
     * The thread is suspended, probably waiting for something
     */
    THREAD_SUSPENDED,

    /**
     * The thread is dead, might be replaced eventually by another thread
     */
    THREAD_DEAD
} thread_state_t;

/**
 * Represents a thread and it's context
 */
typedef struct thread {

    /**
     * The parent of the thread
     */
    struct process* parent;

    /**
     * the thread id (only unique per process)
     */
    size_t tid;

    /**
     * the thread state
     */
    thread_state_t state;

    /**
     * This is the cpu state, it is used to restore the thread
     */
    registers_t cpu_state;
} thread_t;



#endif
