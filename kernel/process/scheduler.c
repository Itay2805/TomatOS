//
// Created by Itay on 26/03/2019.
//

#include <interrupts/interrupts.h>
#include <interrupts/irq.h>

#include <graphics/term.h>

#include <common/string.h>
#include <common/buf.h>

#include <drivers/pit.h>

#include <cpu/rflags.h>

#include "scheduler.h"
#include "process.h"

////////////////////////////////////////////////////////////////////////////
// Idle process
////////////////////////////////////////////////////////////////////////////

/**
 * The idle process, runs when there is nothing else to run
 */
static thread_t idle_thread = {};
static process_t idle_process = {};
static void* idle_process_stack = NULL;

/**
 * just hlt in a loop cause why not
 */
static void idle_thread_loop(void* arg) {
    ((void)arg);
    while(true) {
        hlt();
    }
}

/**
 * Initialize the idle process with a thread per cpu
 */
static error_t idle_process_init() {
    idle_process.address_space = kernel_address_space;
    idle_process.pid = 0;
    idle_process.next_tid = 2;

    idle_thread.parent = &idle_process;
    idle_thread.tid = 1;
    idle_thread.state = THREAD_NORMAL;

    idle_process_stack = kalloc(KB(1));
    buf_push(idle_process.threads, &idle_thread);

    // setup the cpu state of the idle thread
    idle_thread.cpu_state.cs = 8;
    idle_thread.cpu_state.ds = 16;
    idle_thread.cpu_state.ss = 16;
    idle_thread.cpu_state.rflags = RFLAGS_DEFAULT;
    idle_thread.cpu_state.rsp = (uint64_t) (idle_process_stack + KB(1));
    idle_thread.cpu_state.rbp = (uint64_t) (idle_process_stack + KB(1));
    idle_thread.cpu_state.rip = (uint64_t) idle_thread_loop;

    return NO_ERROR;
}

////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////

/**
 * The currently running thread
 */
thread_t* running_thread = NULL;


////////////////////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////////////////////

/**
 * the timer handler, will basically do the scheduling
 * and thread rollup
 */
static error_t handle_timer_interrupt(registers_t* regs) {
    schedule(regs, (int) pit_get_interval());
    return NO_ERROR;
}

void schedule(registers_t* regs, int interval) {
    thread_t* thread_to_run = NULL;
    uint64_t thread_count = 0;
    uint64_t most_watied_time = 0;
    uint64_t time_slice = 0;

    address_space_t addr = vmm_get();
    if(addr != kernel_address_space)
        vmm_set(kernel_address_space);

    // the current thread was suspended
    // save it's state and set that there is no running thread
    if(running_thread != NULL && (running_thread->state == THREAD_SUSPENDED || running_thread->state == THREAD_DEAD)) {
        if(running_thread->state != THREAD_DEAD) {
            running_thread->cpu_state = *regs;
            running_thread->time = 0;
        }
        running_thread = NULL;
    }

    // get the thread count
    // TODO: have this cached somewhere and controller by the thread start or something
    for(process_t** process = processes; process < buf_end(processes); process++) {
        if (*process == NULL) {
            continue;
        }
        for(thread_t** thread = (*process)->threads; thread < buf_end((*process)->threads); thread++) {
            if(*thread != NULL && ((*thread)->state == THREAD_NORMAL || (*thread)->state == THREAD_RUNNING)) {
                thread_count++;
            }
        }
    }

    // calculate the timeslice
    // TODO: Doesn't work from some reason
    // time_slice = MAX(interval, 1000 / thread_count);
    time_slice = (uint64_t) interval;

    // increment the times and choose a thread to run
    for(process_t** it = processes; it < buf_end(processes); it++) {
        if(*it == NULL) continue;
        process_t* process = *it;

        for(thread_t** thread = process->threads; thread < buf_end(process->threads); thread++) {
            // ignore dead threads
            if(*thread == NULL || (*thread)->state == THREAD_DEAD || (*thread)->state == THREAD_SUSPENDED) continue;

            // increment the time
            (*thread)->time += interval;

            if((*thread)->state == THREAD_RUNNING) {
                // this is the running thread,
                // should it continue running?
                if((*thread)->time <= time_slice) {
                    thread_to_run = *thread;
                    break;
                }
            }else {
                // check if this waited the most time
                if((*thread)->time > most_watied_time) {
                    thread_to_run = *thread;
                }
            }
        }
    }

    // if we chose to run the current thread just continue
    if(running_thread != NULL && (thread_to_run == running_thread || thread_to_run == NULL)) {
        if(addr != kernel_address_space)
            vmm_set(addr);
        return;
    }

    // if no thread was found use the idle process
    if(thread_to_run == NULL) {
        // TODO: take the thread for the current cpu
        thread_to_run = idle_process.threads[0];
    }

    // save the state of the current running_thread thread
    if(running_thread != NULL) {
        running_thread->cpu_state = *regs;
        running_thread->state = THREAD_NORMAL;
        running_thread->time = 0;
    }

    // setup the thread to run
    thread_to_run->state = THREAD_RUNNING;
    thread_to_run->time = 0;
    *regs = thread_to_run->cpu_state;
    running_thread = thread_to_run;

    if(kernel_address_space != thread_to_run->parent->address_space) {
        vmm_set(thread_to_run->parent->address_space);
    }
}

error_t scheduler_init() {
    error_t err = NO_ERROR;
    LOG_INFO("configuring PIT to run at 100Hz (10ms)");
    pit_set_interval(10);

    // TODO: Instead of using the interrupt directly we should create a timer wrapper handler or something
    LOG_INFO("setting PIT interrupt handler");
    irq_handlers[IRQ_PIT] = handle_timer_interrupt;

    CHECK_AND_RETHROW(idle_process_init());

cleanup:
    return err;
}

