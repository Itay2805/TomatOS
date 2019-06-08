#include <buf.h>
#include <cpu/msr.h>
#include <string.h>
#include <interrupts/timer.h>
#include "scheduler.h"
#include "thread.h"
#include "process.h"

thread_t* running_thread;

static error_t scheduler_timer(registers_t* regs) {
    error_t err = NO_ERROR;

    // TODO: Schedule

//cleanup:
    return err;
}

error_t scheduler_add(thread_t* thread) {
    // increment refcount
    thread->refcount--;

    // TODO: Add to whatever

    return NO_ERROR;
}

error_t scheduler_init() {
    error_t err = NO_ERROR;

    log_info("Initializing scheduler");
    CHECK_AND_RETHROW(timer_add(scheduler_timer, 10));

cleanup:
    return err;
}