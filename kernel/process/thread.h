#ifndef PROCESS_THREAD_H
#define PROCESS_THREAD_H

#include <common/stdint.h>
#include <interrupts/interrupts.h>

#define DEFAULT_STACK_SIZE MB(4)

// forward declare
struct process;

typedef void(*thread_start_f)(void*);

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

    /**
     * pointer to the start of the stack
     */
    void* stack;

    /**
     * The thread start function, this is where the thread will start running from
     */
    thread_start_f start;

    /**
     * The time that the thread been waiting to run/running
     */
    uint64_t time;
} thread_t;

/**
 * Initialize a thread
 *
 * This function assumes the parent and the start address have been set
 *
 * Note:    1.  DO NOT use this function to create a thread but use the process_start_thread, this function only
 *              initializes the thread context
*           2.  This function needs to do an address space switch to create the stack!
 */
void thread_init(thread_t* thread);

/**
 * Kill a thread
 *
 * Note:    This function needs to do an address space switch to clear the buffer
 */
void thread_kill(thread_t* thread);

#endif
