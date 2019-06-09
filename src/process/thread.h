#ifndef TOMATKERNEL_THREAD_T_H
#define TOMATKERNEL_THREAD_T_H

#include <interrupts/interrupts.h>
#include <map.h>
#include "signal.h"

struct process;

typedef enum thread_status {
    // This is the thread currently running
    THREAD_STATUS_RUNNING,

    // This thread is ready to run
    THREAD_STATUS_READY,

    // This thread has finished running
    THREAD_STATUS_FINISHED,

    // This thread was canceled (killed)
    THREAD_STATUS_CANCELED,

    // This thread was suspended by the aio_suspend syscall
    THREAD_STATUS_SUSPEND,

    // This thread is waiting on join
    THREAD_STATUS_JOIN,

    // This thread is pooled to be used again for another thing
    THREAD_STATUS_POOLED,
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
        uint8_t fpu[512] __attribute__((aligned(16)));
    } state;

    // status specific stuff
    union {
        struct {
            // the thread we are joining
            struct thread* waiting_for;
        } join;
    };

    // the priority of the thread
    int64_t priority;

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
 * Initialize threading
 */
error_t thread_init();

/**
 * Functions like:
 * - thread_join
 *
 * require to use a syscall, this is because it suspends the current thread, a sync operation
 * and we would not want to block the kernel because of it
 */

/**
 * Registers the thread related syscalls
 */
error_t thread_register_syscalls();

/**
 * This allows to create a new thread for the given process
 *
 * @remark
 * Will only set the stack for kernel threads!
 * for user process the creator will have to set the stack.
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
 * @remark
 * The caller has to decrease the refcount by itself
 *
 * @param thread    [IN] The thread to cancel
 */
error_t thread_cancel(thread_t* thread);

/**
 * Add a signal handler to the thread
 *
 * @remark
 * The handler is copied by value, so no need to worry about it
 *
 * @remark
 * The thread is set automatically
 *
 * @param thread    [IN] The thread to add to
 * @param handler    [IN] The handler to add
 */
error_t thread_set_handler(thread_t* thread, signal_handler_t* handler);

/**
 * Will raise a signal on the given thread
 *
 * @param thread    [IN] The thread to raise the signal on
 * @param sig       [IN] The signal to raise
 */
error_t thread_raise(thread_t* thread, uint64_t sig);

/**
 * Try to do a join with a thread, if the thread is dead, will decrement the refcount for it.
 *
 * If the thread is still running, it will simply return an error and won't block.
 *
 * @param thread    [IN]    The thread to join
 * @param retval    [OUT]   The return value of the thread
 */
error_t thread_try_join(thread_t* thread, uint64_t* retval);

/**
 * Will validate the given thread still exists
 *
 * @param thread    [IN] The thread to validate
 */
bool thread_validate(thread_t* thread);

#endif //TOMATKERNEL_THREAD_T_H
