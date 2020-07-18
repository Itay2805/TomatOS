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
 * The next cpu we are gonna put a thread in
 */
static atomic_size_t g_next_cpu = 0;

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
 * it belongs (first is largest pio)
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
    // set max cpus
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
    critical_t crit;

    CHECK(thread != NULL);

    size_t cpu;
    for (cpu = 0; cpu ) {

    }

cleanup:
    return err;
}

static void exec_thread(thread_t* new_thread) {
    //
}

err_t scheduler_tick(system_context_t* ctx) {
    err_t err = NO_ERROR;

    CHECK_FAIL_TRACE("TODO");

cleanup:
    return err;
}
