#include <stb/stb_ds.h>
#include <acpi/tables/madt.h>
#include <interrupts/apic/lapic.h>
#include <smp/percpustorage.h>
#include <helpers/hpet/hpet.h>
#include <common/cpu/msr.h>
#include <common/cpu/cr.h>
#include "scheduler.h"
#include "process.h"
#include "thread.h"

spinlock_t running_threads_lock;
thread_t** running_threads;

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

static error_t scheduler_reschedule_interrupt_wrapper(registers_t* regs, void* context) {
    error_t err = NO_ERROR;

    CHECK_AND_RETHROW(scheduler_reschedule(regs));

cleanup:
    return err;
}

error_t scheduler_reschedule(registers_t* regs) {
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

            // save the cpu regs
            running_thread->context.cpu = *regs;

            // TODO: Save FPU

            // update thread state
            running_thread->running_time = hpet_get_millis();
            running_thread->status = THREAD_STATUS_NORMAL;
            unlock(&running_thread->lock);
        }
        running_threads[get_cpu_index()] = thread;
        unlock(&running_threads_lock);

        // rollup the new thread
        lock(&thread->lock);
        thread->refcount++;

        // set new cpu state
        *regs = thread->context.cpu;

        // TODO: Set new FPU state

        // set the gs/fs base
        _wrmsr(IA32_FS_BASE, thread->context.fs_base);

        // set the stack in the per cpu storage
        get_per_cpu_storage()->kernel_stack = thread->syscall_stack;

        // set the cr3
        if(get_cr3() != thread->process->address_space) {
            set_cr3(thread->process->address_space);
        }

        thread->running_time = hpet_get_millis();
        thread->status = THREAD_STATUS_RUNNING;
        unlock(&thread->lock);

        if(running_thread != NULL) {
            lock(&running_thread->lock);

            // only add to the queue if was actually running
            if(running_thread->status == THREAD_STATUS_RUNNING) {
                arrins(thread_queue, 0, running_thread);
            }

            unlock(&running_thread->lock);
        }
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

            // set new cpu state
            *regs = running_thread->context.cpu;

            // TODO: set new fpu state

            // set the fs base
            _wrmsr(IA32_FS_BASE, running_thread->context.fs_base);

            // set the stack in the per cpu storage
            get_per_cpu_storage()->kernel_stack = running_thread->syscall_stack;

            // set the cr3
            if(get_cr3() != running_thread->process->address_space) {
                set_cr3(running_thread->process->address_space);
            }

            running_thread->running_time = hpet_get_millis();
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

static error_t scheduler_tick(registers_t* regs) {
    error_t err = NO_ERROR;

    // update the running process times
    // TODO: Update more counters or whatever
    if(running_threads[get_cpu_index()] != NULL) {
        lock(&running_threads[get_cpu_index()]->lock);
        running_threads[get_cpu_index()]->running_time += 1;
        unlock(&running_threads[get_cpu_index()]->lock);
    }

    CHECK_AND_RETHROW(scheduler_reschedule(regs));

cleanup:
    return err;
}

static error_t lapic_timer_handler(registers_t* regs, void* context) {
    error_t err = NO_ERROR;

    CHECK_AND_RETHROW(scheduler_reschedule(regs));

cleanup:
    CATCH(lapic_send_eoi());
    return err;
}

static error_t scheduler_start_per_core(registers_t* regs, void* context) {
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
        kfree((void*)thread->stack);
        thread->stack = (uintptr_t)kmalloc(64);

        // set the context again
        thread->context.cpu.rsp = thread->stack + 64;
        thread->context.cpu.rip = (uint64_t)idle_thread_loop;
    }

    // register scheduler interrupts
    log_info("Registering scheduler interrupts");
    log_info("\tstartup (#%d)", INTERRUPT_SCHEDULER_STARTUP);
    CHECK_AND_RETHROW(interrupt_register(INTERRUPT_SCHEDULER_STARTUP, scheduler_start_per_core, NULL));

    log_info("\treschedule (#%d)", INTERRUPT_SCHEDULER_RESCHEDULE);
    CHECK_AND_RETHROW(interrupt_register(INTERRUPT_SCHEDULER_RESCHEDULE, scheduler_reschedule_interrupt_wrapper, NULL));

    timer_vector = interrupt_allocate();
    log_info("Registering timer tick interrupt #%d", timer_vector);
    CHECK_AND_RETHROW(interrupt_register(timer_vector, lapic_timer_handler, NULL));

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
    thread->status = THREAD_STATUS_NORMAL;
    thread->refcount++;
    unlock_preemption(&thread->lock);

    // add to the queue
    lock_preemption(&thread_queue_lock);
    arrins(thread_queue, 0, thread);
    unlock_preemption(&thread_queue_lock);

    return err;
}

error_t scheduler_remove_thread(thread_t* thread, registers_t* regs) {
    error_t err = NO_ERROR;

    // check the thread queue
    lock_preemption(&thread_queue_lock);
    for(int i = 0; i < arrlen(thread_queue); i++) {
        if(thread_queue[i] == thread) {
            // this no longer holds a reference
            lock(&thread_queue[i]->lock);
            thread_queue[i]->refcount--;
            unlock(&thread_queue[i]->lock);

            // remove from the queue
            arrdel(thread_queue, i);
            i--;
        }
    }
    unlock_preemption(&thread_queue_lock);

    // check the running threads
    for(int i = 0; i < arrlen(running_threads); i++) {
        lock(&running_threads[get_cpu_index()]->lock);
        if(running_threads[get_cpu_index()] == thread) {
            unlock(&running_threads[get_cpu_index()]->lock);

            if(i == get_cpu_index()) {
                // removing the thread running on this cpu
                // reschedule
                CHECK_AND_RETHROW(scheduler_reschedule(regs));
            }else {

                // removing the thread running on another cpu
                // send a reschedule ipi
                CHECK_AND_RETHROW(lapic_send_ipi(per_cpu_storage[i].lapic_id, INTERRUPT_SCHEDULER_RESCHEDULE));
            }
        }
    }

cleanup:
    lock_preemption(&running_threads_lock);
    return err;
}