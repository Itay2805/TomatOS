#include "thread.h"

#include "signal.h"
#include "scheduler.h"
#include "process.h"

static error_t syscall_thread_raise(registers_t* regs) {
    error_t err = NO_ERROR;

    // make sure there is an actual running thread
    CHECK(running_thread);

    // get the arguments
    int tid = (int) regs->rdi;
    uint64_t sig = regs->rsi;

    // get the actual thread
    thread_t* thread = process_get_thread(running_thread->parent, tid);
    CHECK_ERROR_TRACE(thread, ERROR_INVALID_ARGUMENT, "Unknown thread id");

    // route the signal
    CHECK_AND_RETHROW(signal_route(running_thread, sig));

cleanup:
    return err;
}
