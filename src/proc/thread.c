#include <mm/mm.h>
#include <mm/stack_allocator.h>
#include <mm/gdt.h>
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
