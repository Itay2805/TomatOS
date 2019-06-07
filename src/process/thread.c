#include <buf.h>
#include <memory/gdt.h>
#include "thread.h"

#include "signal.h"
#include "scheduler.h"
#include "process.h"

thread_t* running_thread;

error_t thread_create(struct process* process, void*(*start_routine)(void*), void* arg, thread_t** thread) {
    error_t err = NO_ERROR;

    // allocate the thread
    thread_t* new_thread = calloc(1, sizeof(thread_t));
    new_thread->tid = ++process->next_tid;
    new_thread->refcount++;
    new_thread->status = THREAD_STATUS_READY;

    // set the segments
    if(kernel_address_space == process->address_space) {
        new_thread->state.cpu.cs = GDT_KERNEL_CODE;
        new_thread->state.cpu.ds = GDT_KERNEL_DATA;
        new_thread->state.cpu.ss = GDT_KERNEL_DATA;

        // for kernel thread also allocate a stack
        new_thread->stack = malloc(MB(2));
    }else {
        new_thread->state.cpu.cs = GDT_USER_CODE;
        new_thread->state.cpu.ds = GDT_USER_DATA;
        new_thread->state.cpu.ss = GDT_USER_DATA;
    }

    // set the start up
    new_thread->state.cpu.rdi = (uint64_t) arg;
    new_thread->state.cpu.rip = (uint64_t) start_routine;

    // add the thread to the threads list of the process
    map_put_from_uint64(&process->threads, (uint64_t) new_thread->tid, new_thread);

    *thread = new_thread;

//cleanup:
//    if(err != NO_ERROR) {
//        free(new_thread);
//    }
    return err;
}

error_t thread_cancel(thread_t* thread) {
    error_t err = NO_ERROR;

    // check arguments
    CHECK_ERROR(thread, ERROR_INVALID_ARGUMENT);

    // free the stack if needed
    if(thread->stack) {
        free(thread->stack);
        thread->stack = NULL;
    }

    // remove join wait for
    if(thread->status == THREAD_STATUS_JOIN) {
        thread->join.waiting_for->refcount--;
    }

    // For pooled thread there is no more we need to do
    if(thread->status != THREAD_STATUS_POOLED) goto cleanup;

    // set the thread as dead
    thread->refcount--;
    thread->status = THREAD_STATUS_CANCELED;

    // only close and free if not pooling this thread and the refcount is 0
    if(thread->refcount == 0) {
        map_put_from_uint64(&thread->parent->threads, (uint64_t) thread->tid, NULL);
        free(thread);
    }

cleanup:
    return err;
}

error_t thread_set_handler(thread_t* thread, signal_handler_t* handler) {
    error_t err = NO_ERROR;

    // make sure the handling thread is the passed one
    handler->thread = thread;
    buf_push(thread->signal_handlers, *handler);

//cleanup:
    return err;
}

error_t thread_raise(thread_t* thread, uint64_t sig) {
    error_t err = NO_ERROR;

    // check arguments
    CHECK_ERROR(thread, ERROR_INVALID_ARGUMENT);

    for(signal_handler_t* handler = thread->signal_handlers; handler < buf_end(running_thread->signal_handlers); handler++) {
        // find the handler and route the signal
        if(handler->value == sig) {
            CHECK_AND_RETHROW(signal_route(handler));
        }
    }

cleanup:
    return err;
}

error_t thread_try_join(thread_t* thread, uint64_t* retval) {
    error_t err = NO_ERROR;

    switch(thread->status) {
        case THREAD_STATUS_FINISHED: {
            thread->refcount--;
            *retval = (uint64_t) thread->retval;
        }

        case THREAD_STATUS_CANCELED: {
            CHECK_FAIL_ERROR(ERROR_THREAD_CANCELED);
        } break;

        default: {
            CHECK_FAIL_ERROR(ERROR_THREAD_RUNNING);
        };
    }

cleanup:
    return err;
}

bool thread_validate(thread_t* thread) {
    error_t err = NO_ERROR;
    uintptr_t addr;

    // make sure this thread actually exists in memory
    CHECK_AND_RETHROW(vmm_virt_to_phys(kernel_address_space, (uintptr_t)thread, &addr));
    CHECK_AND_RETHROW(vmm_virt_to_phys(kernel_address_space, (uintptr_t)((char*)thread) + sizeof(thread_t), &addr));

     // check the parent
    CHECK(process_validate(thread->parent));

    // check that the thread exists in the process
    bool found = false;
    for(uint64_t* key = thread->parent->threads.keys; key < thread->parent->threads.keys + thread->parent->threads.cap; key++) {
        if(*key == (uint64_t) thread) {
            found = true;
            break;
        }
    }
    CHECK(found);

cleanup:
    return err == NO_ERROR;
}
