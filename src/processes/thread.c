#include "thread.h"

#include <stdlib.h>
#include <memory/gdt.h>
#include <cpu/rflags.h>
#include <stb/stb_ds.h>

#include "process.h"
#include "scheduler.h"

error_t create_thread(process_t* process, thread_t** thread) {
    error_t err = NO_ERROR;

    CHECK(thread);

    thread_t* new_thread = kmalloc(sizeof(thread_t));

    // set the thread state
    new_thread->refcount = 1;
    new_thread->status = THREAD_STATUS_WAITING;
    new_thread->process = process;

    // if a kernel thread then create a stack
    if(process->address_space == kernel_address_space) {
        // set the kernel stack
        new_thread->kernel.stack = (uintptr_t) kmalloc(MB(2));
        new_thread->context.cpu.rsp = new_thread->kernel.stack + MB(2);

        // set the segments for kernel thread
        new_thread->context.cpu.ds = GDT_KERNEL_DATA;
        new_thread->context.cpu.ss = GDT_KERNEL_DATA;
        new_thread->context.cpu.cs = GDT_KERNEL_CODE;

        // set the rflags
        new_thread->context.cpu.rflags = RFLAGS_DEFAULT;
    }else {
        // the loader will have to setup the user stack

        // set the segments for user thread
        new_thread->context.cpu.ds = GDT_USER_DATA;
        new_thread->context.cpu.ss = GDT_USER_DATA;
        new_thread->context.cpu.cs = GDT_USER_CODE;

        // set the rflags
        new_thread->context.cpu.rflags = RFLAGS_DEFAULT_USER;
    }

    // set the cpu context
    // TODO: have a get cr3 from address space instead
    new_thread->context.cpu.cr3 = process->address_space;

    // add to the process
    lock_preemption(&process->lock);
    new_thread->tid = process->next_tid++;
    hmput(process->threads, new_thread->tid, new_thread);
    unlock_preemption(&process->lock);

    // set the output
    *thread = new_thread;

cleanup:
    return err;
}

error_t release_thread(thread_t* thread) {
    error_t err = NO_ERROR;

    CHECK(thread);

    lock_preemption(&thread->lock);

    // dev refcount
    thread->refcount--;

    // gotta kill that process
    if(thread->refcount <= 0) {
        // tried to free a thread that has not been killed!
        CHECK_TRACE(thread->status == THREAD_STATUS_DEAD, "Tried to free a thread that has not been killed");

        // free the kernel related stuff
        if(thread->process->address_space == kernel_address_space) {
            kfree((void *) thread->kernel.stack);
        }

        // remove the thread
        lock_preemption(&thread->process->lock);
        hmdel(thread->process->threads, thread->tid);
        unlock_preemption(&thread->process->lock);

        // release the process
        CHECK_AND_RETHROW(release_process(thread->process));

        // delete!
        kfree(thread);
        goto deleted;
    }

cleanup:
    unlock_preemption(&thread->lock);
deleted:
    return err;
}

error_t thread_kill(thread_t* thread, registers_t* regs) {
    error_t err = NO_ERROR;

    CHECK(thread);

    CHECK_AND_RETHROW(scheduler_remove_thread(thread, regs));

    lock_preemption(&thread->lock);

    if(thread->status == THREAD_STATUS_RUNNING) {
        CHECK_AND_RETHROW(scheduler_remove_thread(thread, regs));
    }

    thread->status = THREAD_STATUS_DEAD;

cleanup:
    unlock_preemption(&thread->lock);
    return err;
}
