#include <util/except.h>
#include <sync/lock.h>
#include <arch/cpu.h>
#include <limits.h>
#include "sched.h"

typedef struct cpu_sched_info {
    atomic_int tasks_count;
    list_t tasks;
    lock_t tasks_lock;
} cpu_sched_info_t;

static cpu_sched_info_t* m_cpu_task_info;
static size_t m_cpu_count;

void init_sched(int cpus) {
    m_cpu_count = cpus;

    m_cpu_task_info = kalloc(cpus * sizeof(cpu_sched_info_t));

    for (int cpu = 0; cpu < cpus; cpu++) {
        list_init(&m_cpu_task_info->tasks);
    }
}

static void unlocked_queue_task(int cpu, task_t* task) {
    DEBUG_ASSERT(0 <= cpu && cpu <= m_cpu_count);
    list_push(&m_cpu_task_info[cpu].tasks, &task->schedule_link);
    m_cpu_task_info[cpu].tasks_count++;
}

void queue_task(int cpu, task_t* task) {
    // choose the cpu
    if (cpu == ANY_CPU) {
        int best_cpu = 0;
        int best_count = INT_MAX;
        for (int i = 0; i < m_cpu_count; i++) {
            if (m_cpu_task_info[i].tasks_count < best_count) {
                best_count = m_cpu_task_info[i].tasks_count;
                best_cpu = i;
            }
        }
        cpu = best_cpu;
    }

    // append it
    irq_lock(&m_cpu_task_info[cpu].tasks_lock);
    unlocked_queue_task(cpu, task);
    irq_unlock(&m_cpu_task_info[cpu].tasks_lock);

    if (m_cpu_task_info[cpu].tasks_count == 0) {
        // send ipi
    }
}

void start_schedule_loop() {
    err_t err = NO_ERROR;

    // run with interrupts
    enable_interrupts();

    int cpu = get_cpu_id();
    cpu_sched_info_t* info = &m_cpu_task_info[cpu];
    while (true) {
        // check if empty
        irq_lock(&info->tasks_lock);
        task_t* task = CR(list_pop(&info->tasks), task_t, schedule_link);
        irq_unlock(&info->tasks_lock);

        if (task != NULL) {
            // got something to run, run it
            info->tasks_count--;
            task_handle_t handle = task_handle(task);
            task_resume(handle);

            // check if the task is done
            if (!task_done(handle)) {
                // destroy it
                task_destroy(handle);
            } else {
                // re-add it to this list
                unlocked_queue_task(cpu, task);
            }
        } else {
            // nothing to run, sleep until
            // an interrupt comes
            cpu_sleep();
        }
    }

cleanup:
    ASSERT(false, "Went out of the schedule loop!");
}
