#ifndef PROCESS_THREAD_H
#define PROCESS_THREAD_H

#include <interrupts/interrupts.h>
#include <common/stdint.h>
#include <common/except.h>

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
     * The time that the thread been waiting to run/running
     */
    uint64_t time;
} thread_t;

// TODO: Change to use error_t
thread_t* thread_create(struct process* process, thread_start_f start);

error_t thread_find(struct process* process, int tid, thread_t** thread);

// TODO: Change to use error_t
void thread_kill(thread_t* thread);

#endif
