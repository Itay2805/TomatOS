//
// Created by Itay on 26/03/2019.
//

#include <interrupts/interrupts.h>
#include <interrupts/irq.h>
#include <common/string.h>
#include <drivers/pit.h>
#include <graphics/term.h>
#include <common/buf.h>
#include "scheduler.h"
#include "thread.h"
#include "process.h"
#include "cpu/rflags.h"

////////////////////////////////////////////////////////////////////////////
// Idle process
////////////////////////////////////////////////////////////////////////////

/**
 * The idle process, runs when there is nothing else to run
 */
static process_t idle_process = {};
static void* idle_process_stack = NULL;

/**
 * just hlt in a loop cause why not
 */
static void idle_thread(void* arg) {
    ((void)arg);
    while(true) {
        term_write("Hey rares!\n");
    }
}

/**
 * Initialize the idle process with a thread per cpu
 */
static void idle_process_init() {
    idle_process.address_space = kernel_address_space;
    idle_process.pid = 0;
    idle_process.next_tid = 1;

    idle_process_stack = mm_allocate(&kernel_memory_manager, KB(1));

    // create an idle thread
    // TODO: Make this per cpu
    buf_push(idle_process.threads, (thread_t) {
        .state = THREAD_NORMAL,
        .parent = &idle_process,
        .tid = 1,
        .start = idle_thread,
    });

    // setup the cpu state of the idle thread
    idle_process.threads[0].cpu_state.cs = 8;
    idle_process.threads[0].cpu_state.ds = 16;
    idle_process.threads[0].cpu_state.ss = 16;
    idle_process.threads[0].cpu_state.rflags = RFLAGS_DEFAULT;
    idle_process.threads[0].cpu_state.rsp = (uint64_t) (idle_process_stack + KB(1));
    idle_process.threads[0].cpu_state.rbp = (uint64_t) (idle_process_stack + KB(1));
    idle_process.threads[0].cpu_state.rip = (uint64_t) idle_thread;
}

////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////

/**
 * The currently running thread
 */
static thread_t* running_thread = NULL;


////////////////////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////////////////////

/**
 * the timer handler, will basically do the scheduling
 * and thread rollup
 */
static void schedule(registers_t* regs) {
    thread_t* thread_to_run = NULL;
    uint64_t interval = pit_get_interval();
    uint64_t thread_count = 0;
    uint64_t most_watied_time = 0;
    uint64_t time_slice = 0;

    // get the thread count
    // TODO: have this cached somewhere and controller by the thread start or something
    for(process_t* process = processes; process < buf_end(processes); process++) {
        if (process->pid == DEAD_PROCESS_PID) {
            continue;
        }
        thread_count += buf_len(process->threads);
    }

    // calculate the timeslice
    time_slice = MAX(10, thread_count / 1000);

    // increment the times and choose a thread to run
    for(process_t* process = processes; process < buf_end(processes); process++) {
        if(process->pid == DEAD_PROCESS_PID) continue;

        for(thread_t* thread = process->threads; thread < buf_end(process->threads); thread++) {
            // ignore dead threads
            if(thread->state == THREAD_DEAD) continue;

            // if the thread is suspended no need to actually check times
            if(thread->state == THREAD_SUSPENDED) continue;

            // increment the time
            thread->time += interval;

            if(thread->state == THREAD_RUNNING) {
                // this is the running thread,
                // should it continue running?
                if(thread->time <= time_slice) {
                    thread_to_run = thread;
                }
            }else {
                // check if this waited the most time
                if(thread->time > most_watied_time) {
                    thread_to_run = thread;
                }
            }
        }
    }

    // if we chose to run the current thread just continue
    if(thread_to_run == running_thread || (thread_to_run == NULL && running_thread != NULL)) {
        return;
    }

    // if no thread was found use the idle process
    if(thread_to_run == NULL) {
        // TODO: take the thread for the current cpu
        thread_to_run = &idle_process.threads[0];
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
}

void scheduler_init() {
    term_print("[scheduler_init] configuring PIT to run at 100Hz (10ms)\n");
    pit_set_interval(10);

    // TODO: Instead of using the interrupt directly we should create a timer wrapper handler or something
    term_write("[scheduler_init] setting PIT interrupt handler\n");
    irq_handlers[IRQ_PIT] = schedule;

    idle_process_init();
}

