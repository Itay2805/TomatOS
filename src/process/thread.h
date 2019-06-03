#ifndef TOMATKERNEL_THREAD_T_H
#define TOMATKERNEL_THREAD_T_H

#include <interrupts/interrupts.h>
#include "signal.h"

struct process;

typedef enum thread_status {
    // This is the thread currently running
    THREAD_STATUS_RUNNING,

    // This thread is ready to run
    THREAD_STATUS_READY,

    // This thread is dead (either cancelled or exited)
    THREAD_STATUS_DEAD,

    // This thread was suspended by the aio_suspend syscall
    THREAD_STATUS_SUSPEND,

    // This thread is waiting on join
    THREAD_STATUS_JOIN,

    // This thread is pooled to be used again for another thing
    THREAD_POOLED,
} thread_status_t;

// TODO: We should have a concept of detaching
typedef struct thread {
    // the parent of this thread
    struct process* parent;

    // what the thread is currently doing
    thread_status_t status;

    // the id of this thread
    int tid;

    // the cpu/fpu state of this thread
    struct {
        registers_t cpu;
        char fpu[512];
    } state;

    // status specific stuff
    union {
        struct {
            // the thread we are joining
            struct thread* waiting_for;
        } join;
    };

    // This allows the kernel to track how many stuff are waiting for the thread
    int refcount;

    // the stack of the thread
    // we store it to delete it later
    void* stack;

    // This is the return value of the thread
    void* retval;

    // signal handlers
    signal_handler_t* signal_handlers;
} thread_t;

/**
 * Registers the thread related syscalls
 */
error_t thread_register_syscalls();

/**
 * This allows to create a new thread for the given process
 *
 * @param process           [IN]    The parent process
 * @param start_routine     [IN]    The start routine (where we will jump to)
 * @param arg               [IN]    The argument to pass to the start routine
 * @param thread            [OUT]   The newly created thread (can be NULL)
 */
error_t thread_create(struct process* process, void*(*start_routine)(void*), void* arg, thread_t** thread);

/**
 * Will cancel the given thread
 *
 * @remark
 * Attempting to join a cancelled thread will return ERROR_CANCELLED_THREAD
 *
 * @param thread    [IN] The thread to cancel
 */
error_t thread_cancel(thread_t* thread);

#endif //TOMATKERNEL_THREAD_T_H
