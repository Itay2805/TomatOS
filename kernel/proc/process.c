#include <util/string.h>
#include <mem/mm.h>
#include <stdnoreturn.h>
#include <sync/critical.h>
#include "process.h"
#include "scheduler.h"
#include "event.h"

process_t* CPU_LOCAL g_current_process;
thread_t* CPU_LOCAL g_current_thread;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Process object
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static err_t delete_kernel(handle_meta_t* proc) {
    err_t err = NO_ERROR;

    CHECK_FAIL_TRACE("Tried to kill kernel!");

cleanup:
    return err;
}

process_t g_kernel = {
    .handle_meta = {
        .kind = HANDLE_PROCESS,
        .refcount = 1,
        .dtor = (handle_dtor_t)delete_kernel,
    },
    .pid = 0,
    .handles_lock = INIT_LOCK(),
    .handles = NULL,
    .next_handle = 1,
    .threads = INIT_LIST(g_kernel.threads),
    .threads_lock = INIT_LOCK(),
    .next_tid = 1,
    .name = "kernel",
    .priority = 100,
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The ripper thread - manages threads that need to die
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * The ripper thread
 */
static thread_t* g_ripper_thread;

/**
 * The threads to kill
 */
static list_entry_t g_ripped_threads = INIT_LIST(g_ripped_threads);

/**
 * The lock on the list
 */
static ticket_lock_t g_ripper_lock = INIT_LOCK();

static noreturn void ripper_thread() {
    err_t err = NO_ERROR;

    TRACE("started ripper thread");
    while (true) {
        ticket_lock(&g_ripper_lock);
        while (!list_is_empty(&g_ripped_threads)) {
            thread_t* thread = CR(g_ripped_threads.next, thread_t, scheduler_link);
            list_del(&thread->scheduler_link);

            // remove from the thread list
            ticket_lock(&thread->parent->threads_lock);
            list_del(&thread->link);
            ticket_unlock(&thread->parent->threads_lock);

            // release the process handle
            CHECK_AND_RETHROW(release_handle_meta(&thread->parent->handle_meta));

            // free it
            kfree(thread);
        }
        ticket_unlock(&g_ripper_lock);

        // this thread is woken by the delete_thread function
        g_current_thread->state = STATE_WAITING;
        yield();
    }

cleanup:
    ASSERT_TRACE(false, "Tried to exit ripper thread! (err = %s)",  strerror(err));
}

err_t init_ripper() {
    err_t err = NO_ERROR;

    CHECK_AND_RETHROW(create_thread(&g_ripper_thread, ripper_thread, NULL, "ripper"));
    schedule_thread(g_ripper_thread);

cleanup:
    return err;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Thread object
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* g_state_name[] = {
    [STATE_DEAD] = "Dead",
    [STATE_WAITING] = "Waiting",
    [STATE_RUNNING] = "Running"
};

static err_t delete_thread(handle_meta_t* thread_meta) {
    err_t err = NO_ERROR;
    thread_t* thread = CR(thread_meta, thread_t, handle_meta);

    CHECK_TRACE(thread->state == STATE_DEAD, "tried to kill a none-dead thread (name=%s, state=%s)", thread->name, g_state_name[thread->state]);

    critical_t crit;
    enter_critical(&crit);
    ticket_lock(&g_ripper_lock);
    list_add(&g_ripped_threads, &thread->scheduler_link);
    ticket_unlock(&g_ripper_lock);
    exit_critical(&crit);

    // trigger
    schedule_thread(g_ripper_thread);

cleanup:
    return err;
}

err_t create_thread(thread_t** thread, void(*func)(void* data), void* data, char* name) {
    err_t err = NO_ERROR;

    CHECK(thread != NULL);

    thread_t* new_thread = kalloc(sizeof(thread_t));
    CHECK_ERROR(new_thread != NULL, ERROR_OUT_OF_RESOURCES);

    // this thread references the parent
    g_current_process->handle_meta.refcount++;

    // setup all the meta and shit
    new_thread->handle_meta.kind = HANDLE_THREAD;
    new_thread->handle_meta.refcount = 1;
    new_thread->handle_meta.dtor = delete_thread;
    new_thread->parent = g_current_process;
    new_thread->priority = g_current_process->priority;
    new_thread->tid = g_current_process->next_tid++;
    new_thread->state = STATE_WAITING;
    new_thread->last_cpu = -1;

    // setup the system context
    if ((void *)func > KERNEL_BASE) {
        init_context(&new_thread->system_context, true);
        new_thread->system_context.SP = (uintptr_t)alloc_stack();
        POKE64(new_thread->system_context.SP) = (uintptr_t)exit;
    } else {
        init_context(&new_thread->system_context, false);
        CHECK_FAIL_TRACE("TODO");
    }
    new_thread->system_context.IP = (uintptr_t)func;
    new_thread->system_context.ARG0 = (uintptr_t)data;

    if (name != NULL) {
        memcpy(new_thread->name, name, ARRAY_LENGTH(new_thread->name));
    } else {
        snprintf(new_thread->name, ARRAY_LENGTH(new_thread->name), "%s/%d", g_current_process->name, new_thread->tid);
    }

    // add to the process list
    ticket_lock(&g_current_process->threads_lock);
    list_add_tail(&g_current_process->threads, &new_thread->link);
    ticket_unlock(&g_current_process->threads_lock);

    *thread = new_thread;

cleanup:
    return err;
}

err_t exit() {
    err_t err = NO_ERROR;

    CHECK(g_current_thread != NULL);

    g_current_thread->state = STATE_DEAD;
    yield();

cleanup:
    return err;
}

err_t close_thread(thread_t* thread) {
    err_t err = NO_ERROR;

    CHECK(thread != NULL);
    CHECK_AND_RETHROW(release_handle_meta(&thread->handle_meta));

cleanup:
    return err;
}
