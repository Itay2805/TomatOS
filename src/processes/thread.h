#ifndef TOMATOS_THREAD_H
#define TOMATOS_THREAD_H

#include <stddef.h>
#include <interrupts/interrupts.h>
#include <locks/spinlock.h>

typedef struct process process_t;

typedef enum thread_status {
    THREAD_STATUS_RUNNING,
    THREAD_STATUS_NORMAL,
    THREAD_STATUS_WAITING,
    THREAD_STATUS_DEAD
} thread_status_t;

typedef size_t tid_t;

typedef struct thread {
    // TODO: Move to a kobject
    int refcount;
    spinlock_t lock;

    // the thread id
    tid_t tid;

    // the parent process
    process_t* process;

    // the status
    thread_status_t status;

    // kernel thread info
    struct {
        // the kernel thread struct
        uintptr_t stack;
    } kernel;

    // The context of the thread
    struct {
        registers_t* cpu;
        char fpu[512];
    } context;

    // TODO: Scheduling stuff
    // TODO: Thread signal routing
} thread_t;

/////////////////////////////////////////////
// Kernel object management
/////////////////////////////////////////////

/**
 * Create a new empty thread object
 *
 * @remark
 * For user process the loader needs to set the
 *
 * @param process   [IN]
 * @param thread    [OUT] The out thread
 */
error_t create_thread(process_t* process, thread_t** thread);

/**
 * Will dec the refcount, if it reaches zero will also kill the thread.
 *
 * @remark
 * For killing the thread use the thread_kill instead, this is just for memory management
 *
 * @param thread    [IN] The thread to delete
 */
error_t delete_thread(thread_t* thread);

/////////////////////////////////////////////
// Thread functions
/////////////////////////////////////////////

/**
 * Kill the given thread
 *
 * @remark
 * This will not destroy the object
 *
 * @param thread    [IN] Thread to kill
 */
error_t thread_kill(thread_t* thread);

#endif //TOMATOS_THREAD_H
