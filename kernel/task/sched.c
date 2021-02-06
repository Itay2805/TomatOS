#include "sched.h"

/**
 * This lock protects the scheduler list
 */
static lock_t m_sched_lock = INIT_LOCK(TPL_HIGH_LEVEL);

/**
 * This scheduler list where all the active tasks are
 */
static list_t m_sched_list = { &m_sched_list, &m_sched_list };

/**
 * Bitmap of all the cpus currently idle
 */
static _Atomic(uint64_t) m_trigger_idle = 0;

/**
 * If we have monitor support this will be set to true
 */
static bool m_has_monitor = false;

void init_task_dispatcher() {
    uint32_t ecx = 0;
    cpuid(0x01, NULL, NULL, &ecx, NULL);
    if (ecx & BIT3) {
        TRACE("mwait/monitor available, using for task dispatcher.");
        m_has_monitor = true;
    }
}

err_t queue_task(task_t* task) {
    err_t err = NO_ERROR;

    CHECK(task != NULL);

    acquire_lock(&m_sched_lock);
    list_push(&m_sched_list, &task->schedule_link);
    release_lock(&m_sched_lock);

    // some write to trigger the mwait
    if (m_has_monitor) {
        m_trigger_idle = 1;
    } else {
        lapic_send_fixed_ipi_all_excluding_self(IPI_WAKEUP);
    }

cleanup:
    return err;
}

noreturn void task_dispatcher() {
    if (m_has_monitor) {
        // we are going to use the monitor to wait for a wakeup
        __monitor((size_t)&m_trigger_idle, 0, 0);
    }

    while (true) {
        // get a task from the list
        acquire_lock(&m_sched_lock);
        list_entry_t* task_link = list_pop(&m_sched_list);
        release_lock(&m_sched_lock);

        if (task_link == NULL) {
            // we are going to wait for a trigger
            if (m_has_monitor) {
                __mwait(0, 0);
            } else {
                __hlt();
            }
        } else {
            task_t* task = CR(task_link, task_t, schedule_link);

            // we are now going to be
            task_resume(task);

            // check if the task is done and if
            // so destroy it, otherwise add it
            // back to the queue
            if (!task_done(task)) {
                list_push(&m_sched_list, &task->schedule_link);
            } else {
                task_destroy(task);
            }
        }
    }
}
