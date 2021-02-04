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

err_t queue_task(task_t* task) {
    err_t err = NO_ERROR;

    CHECK(task != NULL);

    acquire_lock(&m_sched_lock);
    list_push(&m_sched_list, &task->schedule_link);
    release_lock(&m_sched_lock);

    // some write to trigger the mwait
    m_trigger_idle = 1;

cleanup:
    return err;
}

noreturn void task_dispatcher() {
    // we are going to use the monitor to wait for a wakeup
    __monitor((size_t)&m_trigger_idle, 0, 0);

    while (true) {
        // get a task from the list
        acquire_lock(&m_sched_lock);
        task_t* task = CR(list_pop(&m_sched_list), task_t, schedule_link);
        release_lock(&m_sched_lock);

        if (task == NULL) {
            // we are going to wait for a trigger
            __mwait((size_t)&m_trigger_idle, 0);
        } else {
            // we are now going to be
            task_resume(task);
        }
    }
}
