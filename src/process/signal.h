#ifndef TOMATKERNEL_SINGAL_H
#define TOMATKERNEL_SINGAL_H

#include <error.h>

struct thread;

typedef enum signal_type {
    // none, there won't be any signal
    SIGNAL_NONE,

    // the signal will create a new thread
    SIGNAL_THREAD,

    // the signal is a callback
    SIGNAL_CALLBACK
} signal_type_t;

typedef struct signal_handler {
    // the type of signal
    signal_type_t type;

    // the value passed to the signal handler
    uint64_t value;

    // The thread
    // If this is a callback, the thread the callback will run on
    // IF this is a thread, the thread calling
    struct thread* thread;

    // The start function, either for the thread or the callback
    void* start;
} signal_handler_t;

/**
 * This will basically return from the signal to the caller
 *
 * this assumes that the stack looks like this:
 * - r11
 * - r10
 * - r9
 * - r8
 * - rcx
 * - rdx
 * - rsi
 * - rdi
 * - rax
 * - rip (return)
 */
extern void signal_return();

/**
 * Will route the signal handler and do whatever it gotta do
 */
error_t signal_route(signal_handler_t* handler);

#endif //TOMATKERNEL_SINGAL_H
