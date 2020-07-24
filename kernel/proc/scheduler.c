#include <mem/mm.h>
#include <sync/critical.h>
#include <arch/timing.h>
#include <stdnoreturn.h>
#include "scheduler.h"

typedef struct run_queue {
    ticket_lock_t lock;
    list_entry_t link;
} run_queue_t;

typedef struct cpu_ctx {
    run_queue_t rqs[2];
    size_t thread_count;
} cpu_ctx_t;

uintptr_t CPU_LOCAL g_idle_stack;

/**
 * The total number of scheduled threads
 */
static atomic_size_t g_num_threads = 0;

/**
 * The max amount of cpus
 */
static size_t g_max_cpu = 0;

/**
 * per-cpu scheduler context
 */
static cpu_ctx_t* g_cpus = NULL;

/**
 * Will add a thread to the given run queue where
 * it belongs (first is largest prio)
 */
static void append_to_queue(run_queue_t* queue, thread_t* thread) {
    thread_t* current;
    FOR_EACH_ENTRY(current, &queue->link, scheduler_link) {
        if (current->priority <= thread->priority) {
            list_add_tail(&current->scheduler_link, &thread->scheduler_link);
        }
    }
    list_add_tail(&queue->link, &thread->scheduler_link);
}

void init_scheduler(size_t cpu_count) {
    TRACE("Initializing scheduler for %d cores", cpu_count);
    g_max_cpu = cpu_count;

    // initialize all of the queues
    g_cpus = kalloc(cpu_count * sizeof(cpu_ctx_t));
    for (int cpu = 0; cpu < cpu_count; cpu++) {
        g_cpus[cpu].thread_count = 0;
        for (int rq = 0; rq < ARRAY_LENGTH(g_cpus[cpu].rqs); rq++) {
            g_cpus[cpu].rqs[rq].lock = INIT_LOCK();
            g_cpus[cpu].rqs[rq].link = INIT_LIST(g_cpus[cpu].rqs[rq].link);
        }
    }
}

static noreturn void idle_loop() {
    while (true) {
        // enable interrupts and wait for one
        // once we get one then yield
        enable_interrupts();
        cpu_sleep();
        yield();
    }
}

void schedule_thread(thread_t* thread) {
    // we now reference this thread
    thread->handle_meta.refcount++;

    size_t cpu = 0;
    if (thread->last_cpu != -1) {
        // already has a last thread, meaning we just woke
        // up the thread
        cpu = thread->last_cpu;
    } else {
        // find cpu with least number of threads
        size_t num = SIZE_MAX;
        for (size_t i = 0; i < g_max_cpu; i++) {
            if (num > g_cpus[i].thread_count) {
                num = g_cpus[i].thread_count;
                cpu = i;
            }
        }

        thread->last_cpu = cpu;
    }

    // add the thread to run queue 0 or 1 depending on the
    // priority of it
    critical_t crit;
    enter_critical(&crit);
    if (thread->priority < 130) {
        ticket_lock(&g_cpus[cpu].rqs[0].lock);
        append_to_queue(&g_cpus[cpu].rqs[0], thread);
        ticket_unlock(&g_cpus[cpu].rqs[0].lock);
    } else {
        ticket_lock(&g_cpus[cpu].rqs[1].lock);
        append_to_queue(&g_cpus[cpu].rqs[1], thread);
        ticket_unlock(&g_cpus[cpu].rqs[1].lock);
    }
    exit_critical(&crit);

    if (thread->last_cpu == -1) {
        // this is a new thread to the scheduler
        g_cpus[cpu].thread_count++;
        g_num_threads++;
    }
}

err_t scheduler_tick(system_context_t* ctx) {
    err_t err = NO_ERROR;
    cpu_ctx_t* cpu_ctx = &g_cpus[g_cpu_id];
    run_queue_t* rq = NULL;

    // handle the current thread
    if (g_current_thread != NULL) {
        if (g_current_thread->state != STATE_DEAD) {
            // store the thread context
            save_context(ctx);

            // setup the last_run and sleep time
            g_current_thread->sleep_time -= (uptime() - g_current_thread->last_run) / 100000;
            g_current_thread->last_run = uptime();

            // add back to a run queue if still needs to run
            if (g_current_thread->state == STATE_RUNNING) {
                if (g_current_thread->priority < 130) {
                    ticket_lock(&g_cpus[g_cpu_id].rqs[0].lock);
                    append_to_queue(&g_cpus[g_cpu_id].rqs[0], g_current_thread);
                    ticket_unlock(&g_cpus[g_cpu_id].rqs[0].lock);
                } else {
                    ticket_lock(&g_cpus[g_cpu_id].rqs[1].lock);
                    append_to_queue(&g_cpus[g_cpu_id].rqs[1], g_current_thread);
                    ticket_unlock(&g_cpus[g_cpu_id].rqs[1].lock);
                }
            }
        } else {
            // remove this thread from the scheduler
            g_current_thread->last_cpu = -1;
            g_cpus[g_cpu_id].thread_count--;
            g_num_threads--;

            // remove the reference completely
            CHECK_AND_RETHROW(close_thread(g_current_thread));
        }
    }

    // find the next run queue with something to run
    for (int i = 0; i < ARRAY_LENGTH(cpu_ctx->rqs); i++) {
        ticket_lock(&cpu_ctx->rqs[i].lock);
        if (!list_is_empty(&cpu_ctx->rqs[i].link)) {
            rq = &cpu_ctx->rqs[i];
            break;
        }
        ticket_unlock(&cpu_ctx->rqs[i].lock);
    }

    if (rq == NULL) {
        // no thread to run, just wait for
        // an interrupt
        g_current_process = &g_kernel;
        g_current_thread = NULL;
        set_address_space(&g_kernel.address_space);
        ctx->IP = (uintptr_t)idle_loop;
        ctx->SP = (uintptr_t)g_idle_stack;
    } else {
        // dequeue a thread
        g_current_thread = CR(rq->link.next, thread_t, scheduler_link);
        list_del(&g_current_thread->scheduler_link);

        // don't forget to unlock it
        ticket_unlock(&rq->lock);

        // set everything up
        g_current_process = g_current_thread->parent;
        g_current_thread->state = STATE_RUNNING;

        // setup for execution
        restore_context(ctx);

        // calculate the priority based on the avg sleep time of the thread
        uint64_t delta = (uptime() - g_current_thread->last_run) / 100;
        g_current_thread->sleep_time += delta;
        g_current_thread->sleep_time = MIN(g_current_thread->sleep_time, 10);
        g_current_thread->priority = MAX(100, MIN(g_current_thread->priority - g_current_thread->sleep_time + 5, 139));

        // calculate the last time based on priority
        uint64_t timeslice = 0;
        if (g_current_thread->priority < 120) {
            timeslice = (140 - g_current_thread->priority) * 20;
        } else {
            timeslice = (140 - g_current_thread->priority) * 5;
        }
        set_next_scheduler_tick(timeslice);

        // set the last time runnning
        g_current_thread->last_run = uptime();
    }

cleanup:
    return err;
}

void startup_scheduler() {
    // simply fire scheduler tick interrupt manually
    enable_interrupts();
    yield();
}
