#include <mem/mm.h>
#include <sync/critical.h>
#include "scheduler.h"

typedef struct run_queue {
    ticket_lock_t lock;
    list_entry_t link;
} run_queue_t;

typedef struct cpu_ctx {
    run_queue_t rqs[3];
    size_t thread_count;
} cpu_ctx_t;

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

err_t schedule_thread(thread_t* thread) {
    err_t err = NO_ERROR;

    // we now reference this thread
    thread->handle_meta.refcount++;

    // find cpu with least number of threads
    size_t cpu = 0;
    size_t num = SIZE_MAX;
    for (size_t i = 0; i < g_max_cpu; i++) {
        if (num > g_cpus[i].thread_count) {
            num = g_cpus[i].thread_count;
            cpu = i;
        }
    }

    // add the thread to run queue 1 of the given cpu
    // this has to be in a critical section because we
    // can schedule threads from an interrupt handler
    critical_t crit;
    enter_critical(&crit);
    ticket_lock(&g_cpus[cpu].rqs[1].lock);
    append_to_queue(&g_cpus[cpu].rqs[1], thread);
    ticket_unlock(&g_cpus[cpu].rqs[1].lock);
    exit_critical(&crit);

    // increase the amount of threads
    g_cpus[cpu].thread_count++;
    g_num_threads++;

    return err;
}

err_t deschedule_thread(thread_t* process) {
    err_t err = NO_ERROR;

    CHECK_FAIL_TRACE("TODO: Implement this");

cleanup:
    return err;
}

static void finish_exec_thread(thread_t* old_thread, system_context_t* ctx) {

}

/**
 * Execute the thread, this includes:
 *  - setting the thread context
 *  - setting per-thread globals
 *  - setting the next time a scheduler tick
 */
static void exec_thread(thread_t* new_thread, system_context_t* ctx) {
    TRACE("Going to exec thread `%s`", new_thread->name);
    set_next_scheduler_tick(100);
}

static void exec_idle(system_context_t* ctx) {
    TRACE("Going to exec idle");
}

err_t scheduler_tick(system_context_t* ctx) {
    err_t err = NO_ERROR;
    cpu_ctx_t* cpu_ctx = &g_cpus[g_cpu_id];
    run_queue_t* rq = NULL;

    // handle the current thread
    if (g_current_thread != NULL) {
        if (g_current_thread->state != STATE_RUNNING) {
            CHECK_AND_RETHROW(deschedule_thread(g_current_thread));
        }

        if (g_current_thread->state != STATE_DEAD) {
            finish_exec_thread(g_current_thread, ctx);
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
        // no thread to run
        exec_idle(ctx);
    } else {
        // dequeue a thread
        thread_t* new_thread = CR(rq->link.next, thread_t, scheduler_link);
        list_del(&new_thread->scheduler_link);

        // don't forget to unlock it
        ticket_unlock(&rq->lock);

        // setup for execution
        exec_thread(new_thread, ctx);

        // this is the new thread
        g_current_thread = new_thread;
        g_current_thread->last_rq = (rq - cpu_ctx->rqs);
        g_current_thread->last_cpu = g_cpu_id;
        g_current_thread->state = STATE_RUNNING;
    }

cleanup:
    return err;
}

void startup_scheduler() {
    // simply fire scheduler tick interrupt manually
    enable_interrupts();
    asm("int $0x20");
}
