#include <stb/stb_ds.h>
#include <smp/cpustorage.h>
#include "scheduler.h"
#include "process.h"
#include "thread.h"

static thread_t** running_threads;

///////////////////////////////////////////////////////////////
// Idle process
///////////////////////////////////////////////////////////////

static process_t* idle_process;

static void idle_thread_loop() {
    while(true) {
        _hlt();
    }
}

error_t scheduler_init() {
    error_t err = NO_ERROR;

    arrsetlen(running_threads, get_core_count());

    // create the idle process
    create_process(kernel_address_space, &idle_process);

    // initialize the idle threads per core
    for(int i = 0; i < get_core_count(); i++) {
        // create the thread
        thread_t* thread = NULL;
        CHECK_AND_RETHROW(create_thread(kernel_process, &thread));

        // only really ever gonna loop, so allocate a smaller stack
        kfree((void*)thread->kernel.stack);
        thread->kernel.stack = (uintptr_t)kmalloc(64);

        // set the context again
        thread->context.cpu.rsp = thread->kernel.stack;
        thread->context.cpu.rip = (uint64_t)idle_thread_loop;
    }

cleanup:
    return err;
}

error_t scheduler_start_per_core(registers_t* regs) {
    return NO_ERROR;
}

error_t scheduler_kill_thread(thread_t* thread) {
    // TODO: this
    return ERROR_NOT_IMPLEMENTED;
}