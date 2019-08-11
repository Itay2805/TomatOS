#include <stb/stb_ds.h>
#include <acpi/tables/madt.h>
#include <interrupts/apic/lapic.h>
#include <smp/percpustorage.h>
#include <drivers/hpet/hpet.h>
#include "scheduler.h"
#include "process.h"
#include "thread.h"

static spinlock_t running_threads_lock;
static thread_t** running_threads;

static spinlock_t thread_queue_lock;
static thread_t** thread_queue;

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

    lock(&thread_queue_lock);
    if(arrlen(thread_queue) != 0) {
        // we have threads waiting to run, switch!
        thread_t* thread = arrpop(thread_queue);

        // switch the thread
        lock(&running_threads_lock);
        thread_t* running_thread = running_threads[get_cpu_index()];
        if(running_thread != NULL) {

            // do la switch
            // save the context of the first one
            lock(&running_thread->lock);
            running_thread->refcount--;
            running_thread->context.cpu = *regs;
            // TODO: Save FPU
            running_thread->last_time = hpet_get_millis();
            running_thread->status = THREAD_STATUS_NORMAL;
            unlock(&running_thread->lock);
        }
        running_threads[get_cpu_index()] = thread;

        // rollup the new thread
        lock(&thread->lock);
        thread->refcount++;
        *regs = thread->context.cpu;
        thread->last_time = hpet_get_millis();
        thread->status = THREAD_STATUS_RUNNING;
        unlock(&thread->lock);

        if(running_thread != NULL) {
            // queue the thread back
            arrins(thread_queue, 0, running_thread);
        }

        unlock(&running_threads_lock);
    }else {
        /*
         * Make sure that we always have a thread to run
         */
        lock(&running_threads_lock);
        thread_t* running_thread = running_threads[get_cpu_index()];

        // no thread to run
        if(running_thread == NULL) {
            // make it run the idle thread

            lock(&idle_process->lock);
            log_warn("\tNo threads to run, running idle thread");

            running_thread = hmget(idle_process->threads, get_cpu_index());
            lock(&running_thread->lock);
            running_thread->refcount++;
            *regs = running_thread->context.cpu;
            running_thread->last_time = hpet_get_millis();
            running_thread->status = THREAD_STATUS_RUNNING;
            unlock(&running_thread->lock);

            unlock(&idle_process->lock);
        }
        unlock(&running_threads_lock);
    }
    unlock(&thread_queue_lock);

//cleanup:
    return err;
}

static error_t lapic_timer_handler(registers_t* regs) {
    error_t err = NO_ERROR;

    CHECK_AND_RETHROW(scheduler_tick(regs));

cleanup:
    CATCH(lapic_send_eoi());
    return err;
}

static error_t scheduler_start_per_core(registers_t* regs) {
    error_t err = NO_ERROR;

    log_info("Doing scheduler startup on AP #%d", get_per_cpu_storage()->processor_id);

    // set the timer
    CHECK_AND_RETHROW(lapic_set_timer(1, timer_vector));

    // do the first scheduling
    CHECK_AND_RETHROW(scheduler_tick(regs));

    // starts allow to get interrupts
    _sti();

cleanup:
    CATCH(lapic_send_eoi());
    return err;
}

error_t scheduler_init() {
    error_t err = NO_ERROR;

    arrsetlen(running_threads, arrlen(per_cpu_storage));

    // create the idle process
    create_process(kernel_address_space, &idle_process);

    // initialize the idle threads per core
    for(int i = 0; i < arrlen(per_cpu_storage); i++) {
        // create the thread
        thread_t* thread = NULL;
        CHECK_AND_RETHROW(create_thread(idle_process, &thread));

        // only really ever gonna loop, so allocate a smaller stack
        kfree((void*)thread->kernel.stack);
        thread->kernel.stack = (uintptr_t)kmalloc(64);

        // set the context again
        thread->context.cpu.rsp = thread->kernel.stack;
        thread->context.cpu.rip = (uint64_t)idle_thread_loop;
    }

    // TODO: We could allocate it dynamically and free it once we started all schedulers
    log_info("Registering scheduler startup interrupt #%d", INTERRUPT_SCHEDULER_STARTUP);
    CHECK_AND_RETHROW(interrupt_register(INTERRUPT_SCHEDULER_STARTUP, scheduler_start_per_core));

    timer_vector = interrupt_allocate();
    log_info("Registering timer tick interrupt #%d", timer_vector);
    CHECK_AND_RETHROW(interrupt_register(timer_vector, lapic_timer_handler));

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

        CHECK_AND_RETHROW(lapic_send_ipi(madt_lapics[i]->id, INTERRUPT_SCHEDULER_STARTUP));
    }

cleanup:
    return err;
}

error_t scheduler_queue_thread(thread_t* thread) {
    error_t err = NO_ERROR;

    // this now holds a reference
    lock_preemption(&thread->lock);
    thread->refcount++;
    unlock_preemption(&thread->lock);

    // add to the queue
    lock_preemption(&thread_queue_lock);
    arrins(thread_queue, 0, thread);
    unlock_preemption(&thread_queue_lock);

    return err;
}

error_t scheduler_remove_thread(thread_t* thread) {
    error_t err = NO_ERROR;
    lock_preemption(&thread_queue_lock);

    for(int i = 0; i < arrlen(thread_queue); i++) {
        if(thread_queue[i] == thread) {
            // this no longer holds a reference
            lock_preemption(&thread_queue[i]->lock);
            thread_queue[i]->refcount--;
            unlock_preemption(&thread_queue[i]->lock);

            // remove from the queue
            arrdel(thread_queue, i);
            i--;
        }
    }

    // TODO: Check for the running threads

//cleanup:
    unlock_preemption(&thread_queue_lock);
    return err;
}