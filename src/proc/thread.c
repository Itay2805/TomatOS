#include <mm/mm.h>
#include <mm/stack_allocator.h>
#include <mm/gdt.h>
#include <intr/apic/lapic.h>
#include "thread.h"
#include "sched.h"

err_t spawn_thread(process_t* parent, uintptr_t rip, uintptr_t stack, thread_t** new_thread) {
    err_t err = NO_ERROR;
    thread_t* thread = NULL;

    CHECK(parent != NULL);

    spinlock_acquire(&parent->threads_lock);

    // allocate it
    thread = mm_allocate(sizeof(thread_t));
    thread->tid = parent->tid_gen++;
    thread->parent = parent;
    thread->state = STATE_WAITING;
    thread->thread_tpl = TPL_APPLICATION;

    // if not a kernel process allocate a kernel stack
    if (parent != &kernel_process) {
        thread->kernel_stack = allocate_stack();
        thread->cpu_context.ss = GDT_USER_DATA;
        thread->cpu_context.ds = GDT_USER_DATA;
        thread->cpu_context.cs = GDT_USER_CODE;
    } else {
        thread->cpu_context.ss = GDT_KERNEL_DATA;
        thread->cpu_context.ds = GDT_KERNEL_DATA;
        thread->cpu_context.cs = GDT_KERNEL_CODE;
    }

    // set the initial context
    thread->cpu_context.rip = rip;
    thread->cpu_context.rsp = stack - 16;
    thread->cpu_context.rflags.always_one = 1;
    thread->cpu_context.rflags.IF = 1;
    thread->cpu_context.rflags.ID = 1;

    // insert it
    insert_tail_list(&parent->threads_list, &thread->thread_link);

    if (new_thread) {
        *new_thread = thread;
    } else {
        CHECK_AND_RETHROW(queue_thread(thread));
    }

cleanup:
    // if failed free the thread and related resources
    if (IS_ERROR(err)) {
        if (parent != &kernel_process && thread != NULL) {
            if (thread->thread_link.next != NULL) {
                remove_entry_list(&thread->thread_link);
            }
            free_stack(thread->kernel_stack);
            mm_free(thread);
        }
    }

    spinlock_release(&parent->threads_lock);

    return err;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// syscall stuff
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

err_t sys_thrd_spawn(syscall_context_t* ctx) {
    err_t err = NO_ERROR;
    thread_t* new_thread = NULL;

    // get params
    process_t* parent = g_current_thread->parent;
    uintptr_t rip = ctx->arg1;
    uintptr_t stack = ctx->arg2;
    uintptr_t flags = ctx->arg3;

    // spawn the new thread
    CHECK_AND_RETHROW(spawn_thread(parent, rip, stack, &new_thread));

    // only schedule if not in a paused state
    if (!(flags & THRD_SPAWN_WAIT)) {
        CHECK_AND_RETHROW(queue_thread(new_thread));
    }

    // setup the tid
    ctx->ret_value = new_thread->tid;

cleanup:
    if (IS_ERROR(err)) {
        if (new_thread != NULL) {
            ASSERT(!"Failed to queue a thread!");
        }
    }
    return err;
}

err_t sys_thrd_set_opt(syscall_context_t* ctx) {
    err_t err = NO_ERROR;

    tid_t tid = ctx->arg1;
    uintptr_t opt = ctx->arg2;
    uintptr_t value = ctx->arg3;

    // figure the thread
    thread_t* thread = NULL;
    if (tid == 0) {
        thread = g_current_thread;
    }

    // do whatever
    switch (opt) {
        case THRD_OPT_STATE: {
            switch (value) {
                case THRD_STATE_WAITING: {
                    CHECK_AND_RETHROW(queue_thread(thread));
                } break;

                case THRD_STATE_NORMAL: {
                    thread_state_t old_state = thread->state;
                    // only set to waiting on these states
                    if (old_state == STATE_NORMAL || old_state == STATE_WAITING) {
                        thread->state = STATE_WAITING;
                    }

                    // send an ipi to yield
                    if (old_state == STATE_RUNNING) {
                        send_fixed_ipi(thread->apic_id, IPI_YIELD);
                    }
                } break;

                default:
                    CHECK_FAIL_ERROR(ERROR_INVALID_PARAM);
            }
        } break;

        default:
            // invalid option
            CHECK_FAIL_ERROR(ERROR_INVALID_PARAM);
    }

cleanup:
    return err;
}

err_t sys_thrd_raise_tpl(syscall_context_t* ctx) {
    err_t err = NO_ERROR;
    tpl_t new_tpl = ctx->arg1;
    tpl_t old_tpl = get_tpl();

    // check that the tpl is valid
    CHECK_ERROR(new_tpl <= TPL_USER_HIGH, ERROR_INVALID_TPL);
    CHECK_ERROR(new_tpl >= old_tpl, ERROR_INVALID_TPL);

    // return the current tpl
    ctx->ret_value = raise_tpl(new_tpl);

cleanup:
    return err;
}

err_t sys_thrd_restore_tpl(syscall_context_t* ctx) {
    err_t err = NO_ERROR;
    tpl_t new_tpl = ctx->arg1;
    tpl_t old_tpl = get_tpl();

    // check that the tpl is valid
    CHECK_ERROR(new_tpl <= TPL_USER_HIGH, ERROR_INVALID_TPL);
    CHECK_ERROR(new_tpl <= old_tpl, ERROR_INVALID_TPL);

    // restore the tpl
    restore_tpl(new_tpl);

cleanup:
    return err;
}
