#include <stb/stb_ds.h>
#include <smp/cpustorage.h>
#include <acpi/tables/madt.h>
#include <interrupts/apic/lapic.h>
#include "scheduler.h"
#include "process.h"
#include "thread.h"

static spinlock_t running_threads_lock;
static thread_t** running_threads;

static spinlock_t thread_queue_lock;
static thread_t* thread_queue;

static uint8_t timer_vector;


///////////////////////////////////////////////////////////////
// Idle process
///////////////////////////////////////////////////////////////

static process_t* idle_process;

static void idle_thread_loop() {
    while(true) {
        _hlt();
    }
}
///////////////////////////////////////////////////////////////
// The scheduler
///////////////////////////////////////////////////////////////

static error_t scheduler_tick(registers_t* regs) {
    error_t err = NO_ERROR;

    lock(&running_threads_lock);
    unlock(&running_threads_lock);

cleanup:

    // no deadlocks please
    if(running_threads_lock.locked) unlock(&running_threads_lock);
    if(thread_queue_lock.locked) unlock(&thread_queue_lock);

    return err;
}

static error_t scheduler_start_per_core(registers_t* regs) {
    error_t err = NO_ERROR;

    log_info("Doing scheduler startup on core #%d", cpu_index);

    // set the timer
    CHECK_AND_RETHROW(lapic_set_timer(1, timer_vector));

    // do the first scheduling
    CHECK_AND_RETHROW(scheduler_tick(regs));

cleanup:
    return err;
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

    // TODO: We could allocate it dynamically and free it once we started all schedulers
    log_info("Registering scheduler startup interrupt #%d", INTERRUPT_SCHEDULER_STARTUP);
    interrupt_register(INTERRUPT_SCHEDULER_STARTUP, scheduler_start_per_core);

    timer_vector = interrupt_allocate();
    log_info("Registering timer tick interrupt #%d", timer_vector);
    interrupt_register(timer_vector, scheduler_tick);

cleanup:
    return err;
}

error_t scheduler_kickstart() {
    error_t err = NO_ERROR;

    CHECK(idle_process);

    for(int i = 0; i < arrlen(madt_lapics); i++) {
        if(!madt_lapics[i]->processor_enabled) continue;

        // TODO: Remove once we started all cores
        if(madt_lapics[i]->id != lapic_get_id()) continue;

        // TODO: Send IPI to start scheduling per core (should include this core)
        CHECK_AND_RETHROW(lapic_send_ipi(madt_lapics[i]->id, INTERRUPT_SCHEDULER_STARTUP));
    }

cleanup:
    return err;
}

error_t scheduler_kill_thread(thread_t* thread) {
    error_t err = NO_ERROR;
    lock(thread_queue_lock);

cleanup:
    return err;
}