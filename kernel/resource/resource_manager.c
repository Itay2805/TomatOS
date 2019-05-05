#include "resource_manager.h"

#include <common/buf.h>
#include <common/string.h>
#include <interrupts/interrupts.h>
#include <process/thread.h>
#include <process/scheduler.h>
#include <process/syscall.h>

#include "resource.h"

resource_provider_t* providers = NULL;

extern void syscall_queue_trampoline();

static error_t syscall_provider_handler_finished(registers_t* regs) {
    error_t err = NO_ERROR;
    process_t* process = NULL;
    thread_t* thread = NULL;

    // check the arguments
    CHECK_AND_RETHROW(process_find(regs->rdi, &process));
    CHECK_AND_RETHROW(thread_find(process, regs->rsi, &thread));

    // resume the thread that was waiting
    thread->cpu_state.rax = regs->rcx;
    thread->state = THREAD_NORMAL;
    thread->time = 0;

cleanup:
    // kill the thread and reschedule even if we failed
    thread_kill(running_thread);
    schedule(regs, 0);

    return err;
}

// this handles the all the syscalls that get the resource as the first parameter
//
// NOTE: This will not copy anything to the kernel, which means
//       each open handler will need to copy to the kernel the descriptor
//       by itself, same with writing back to the user
static error_t syscall_provider_handle(registers_t* regs) {
    error_t err = NO_ERROR;
    process_t* running_process = running_thread->parent;
    process_t* provider_process = NULL;
    thread_t* provider_thread = NULL;
    resource_provider_t* provider = NULL;
    char* stack = NULL;

    running_thread->state = THREAD_SUSPENDED;

    // TODO: Somehow map between the given resource to the provider
    CHECK_FAIL();

    CHECK_AND_RETHROW(process_find(provider->pid, &provider_process));
    switch(regs->rax) {
        case SYSCALL_OPEN:
            provider_thread = process_start_thread(provider_process, provider->open);
            provider_thread->cpu_state.rdi = regs->rdi;
            provider_thread->cpu_state.rsi = regs->rsi;

        case SYSCALL_CLOSE:
            provider_thread = process_start_thread(provider_process, provider->close);
            provider_thread->cpu_state.rdi = regs->rdi;

        case SYSCALL_READ:
        case SYSCALL_WRITE:
        case SYSCALL_SELECT:
        case SYSCALL_POLL:
        case SYSCALL_WAIT:            

        // TODO: Implement the rest
        default:
            CHECK_FAIL_ERROR(ERROR_INVALID_SYSCALL);
    }
    provider_thread->cpu_state.rdi = regs->rdi;

    // we are going to allocate a small stack
    CHECK_AND_RETHROW(mm_allocate_aligned(&kernel_memory_manager, KB(4), KB(4), &provider_thread->cpu_state.rsp));
    provider_thread->cpu_state.rbp = stack + KB(4);
    provider_thread->cpu_state.rsp = stack + KB(4);

    schedule(regs, 0);
cleanup:
    if(IS_ERROR(err)) {
        regs->rax = -err;
        if(provider_thread != NULL) thread_kill(provider_thread);
        if(stack != NULL) mm_free(&kernel_memory_manager, stack);
    }

    return err;
}

error_t resource_manager_init() {
    syscalls[SYSCALL_PROVIDER_HANDLER_FINISHED] = syscall_provider_handler_finished;

    syscalls[SYSCALL_OPEN] = syscall_provider_handle;
    syscalls[SYSCALL_CLOSE] = syscall_provider_handle;
    syscalls[SYSCALL_READ] = syscall_provider_handle;
    syscalls[SYSCALL_WRITE] = syscall_provider_handle;
    syscalls[SYSCALL_SELECT] = syscall_provider_handle;
    syscalls[SYSCALL_POLL] = syscall_provider_handle;
    syscalls[SYSCALL_WAIT] = syscall_provider_handle;
}

error_t resource_manager_register_provider(resource_provider_t* provider) {
    term_print("[resource_manager_register] registered provider for '%s' (pid=%d)", provider->scheme, provider->pid);
    buf_push(providers, *provider);
    return NO_ERROR;
}

error_t resource_manager_get_provider_by_scheme(const char* scheme, resource_provider_t** provider) {
    error_t err = NO_ERROR;
    resource_provider_t* found = NULL;

    for(resource_provider_t* it = providers; it < buf_end(providers); it++) {
        if(strcmp(it->scheme, scheme) == 0) {
            found = it;
            break;
        }
    }

    CHECK_ERROR(found != NULL, ERROR_NOT_FOUND);

cleanup:
    return err;
}

error_t resource_manager_get_provider_by_pid(int pid, resource_provider_t** provider) {
    error_t err = NO_ERROR;
    resource_provider_t* found = NULL;

    for(resource_provider_t* it = providers; it < buf_end(providers); it++) {
        if(it->pid == pid) {
            found = it;
            break;
        }
    }

    CHECK_ERROR(found != NULL, ERROR_NOT_FOUND);

cleanup:
    return err;
}
