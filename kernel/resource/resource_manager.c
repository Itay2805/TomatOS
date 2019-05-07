#include "resource_manager.h"

#include <interrupts/interrupts.h>

#include <process/scheduler.h>
#include <process/syscall.h>
#include <process/process.h>
#include <process/thread.h>

#include <common/string.h>
#include <common/buf.h>

#include <memory/gdt.h>

#include "resource.h"

resource_provider_t** providers = NULL;

extern void dispatch_resource_call_trampoline();

static error_t syscall_provider_handler_finished(registers_t* regs) {
    error_t err = NO_ERROR;
    process_t* process = (process_t*)regs->rdi;
    thread_t* thread = NULL;

    // check the arguments
    CHECK_AND_RETHROW(thread_find(process, regs->rsi, &thread));

    // only providers can call the provider handler finished
    // because only kernel processes can be providers we first filter by kernel processes
    // and if it is a kernel process we check if it is a provider
    CHECK_ERROR(running_thread->parent->kernel, ERROR_INVALID_SYSCALL);
    CHECK_ERROR(!IS_ERROR(resource_manager_get_provider_by_pid(running_thread->parent->pid, NULL)), ERROR_INVALID_SYSCALL);


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
static error_t dispatch_resource_call(registers_t* regs) {
    error_t err = NO_ERROR;
    process_t* running_process = NULL;
    process_t* provider_process = NULL;
    thread_t* provider_thread = NULL;
    resource_provider_t* provider = NULL;
    char* stack = NULL;
    char* scheme = NULL;

    // get the running process
    CHECK(running_thread != NULL);
    running_process = running_thread->parent;

    switch(regs->rax) {
        case SYSCALL_OPEN: {
            resource_descriptor_t descriptor = {0};
            size_t len = 0;

            // first copy the descriptor
            CHECK_AND_RETHROW(vmm_copy_to_kernel(running_process->address_space, (void*)regs->rdi, &descriptor, sizeof(resource_descriptor_t)));
            // get the scheme name length, allocate a buffer to store it, and read the scheme
            CHECK_AND_RETHROW(vmm_copy_string_to_kernel(running_process->address_space, descriptor.scheme, NULL, &len));
            CHECK_AND_RETHROW(mm_allocate(&kernel_memory_manager, len, (void**)&scheme));
            CHECK_AND_RETHROW(vmm_copy_string_to_kernel(running_process->address_space, descriptor.scheme, scheme, &len));

            // get the actual provider
            CHECK_AND_RETHROW(resource_manager_get_provider_by_scheme(scheme, &provider));
        } break;

        case SYSCALL_READ:
        case SYSCALL_WRITE:
        case SYSCALL_SEEK:
        case SYSCALL_TELL:
        case SYSCALL_POLL:
            CHECK_AND_RETHROW(resource_manager_get_provider_by_resource(running_process, regs->rdi, &provider));
            break;

        default:
            CHECK_FAIL_ERROR(ERROR_NOT_IMPLEMENTED);
    }

    // find the provider process and create a new thread on it
    CHECK_AND_RETHROW(process_find(provider->pid, &provider_process));
    provider_thread = process_start_thread(provider_process, dispatch_resource_call_trampoline);

    switch(regs->rax) {
        case SYSCALL_OPEN:
            CHECK_ERROR((uint64_t)provider->open != NULL, ERROR_NOT_IMPLEMENTED);
            provider_thread->cpu_state.rax = (uint64_t)provider->open;
            provider_thread->cpu_state.rdx = regs->rdi; // resource_description_t* description
            provider_thread->cpu_state.rcx = regs->rsi; // resource_t* out_resource
            break;

        case SYSCALL_CLOSE:
            CHECK_ERROR((uint64_t)provider->close != NULL, ERROR_NOT_IMPLEMENTED);
            provider_thread->cpu_state.rax = (uint64_t)provider->close;
            provider_thread->cpu_state.rdx = regs->rdi; // resource_t resource
            break;

        case SYSCALL_READ:
            CHECK_ERROR((uint64_t)provider->read != NULL, ERROR_NOT_IMPLEMENTED);
            provider_thread->cpu_state.rax = (uint64_t)provider->read;
            provider_thread->cpu_state.rdx = regs->rdi; // resource_t resource
            provider_thread->cpu_state.rcx = regs->rsi; // char* buffer
            provider_thread->cpu_state.r8 = regs->rdx; // size_t len
            break;

        case SYSCALL_WRITE:
            CHECK_ERROR((uint64_t)provider->write != NULL, ERROR_NOT_IMPLEMENTED);
            provider_thread->cpu_state.rax = (uint64_t)provider->write;
            provider_thread->cpu_state.rdx = regs->rdi; // resource_t resource
            provider_thread->cpu_state.rcx = regs->rsi; // char* buffer
            provider_thread->cpu_state.r8 = regs->rdx; // size_t len
            break;

        case SYSCALL_TELL:
            CHECK_ERROR((uint64_t)provider->tell != NULL, ERROR_NOT_IMPLEMENTED);
            provider_thread->cpu_state.rax = (uint64_t)provider->tell;
            provider_thread->cpu_state.rdx = regs->rdi; // resource_t resource
            provider_thread->cpu_state.rcx = regs->rsi; // seek_t relative_to
            provider_thread->cpu_state.r8 = regs->rdx; // size_t pos
            break;

        case SYSCALL_SEEK:
            CHECK_ERROR((uint64_t)provider->seek != NULL, ERROR_NOT_IMPLEMENTED);
            provider_thread->cpu_state.rax = (uint64_t)provider->seek;
            provider_thread->cpu_state.rdx = regs->rdi; // resource_t resource
            provider_thread->cpu_state.rcx = regs->rsi; // size_t* pos
            break;

        case SYSCALL_POLL:
            CHECK_ERROR((uint64_t)provider->poll != NULL, ERROR_NOT_IMPLEMENTED);
            provider_thread->cpu_state.rax = (uint64_t)provider->poll;
            // TODO: Implement this
            CHECK_FAIL_ERROR(ERROR_NOT_IMPLEMENTED);

        default:
            CHECK_FAIL_ERROR(ERROR_NOT_IMPLEMENTED);
    }
    provider_thread->cpu_state.rdi = (uint64_t) running_process;
    provider_thread->cpu_state.rsi = running_thread->tid;

    // we are going to allocate a small stack
    CHECK_AND_RETHROW(mm_allocate_aligned(&kernel_memory_manager, KB(4), KB(4), (void**)&stack));
    provider_thread->cpu_state.rbp = (uintptr_t)stack + KB(4);
    provider_thread->cpu_state.rsp = (uintptr_t)stack + KB(4);

    // save the state of the thread
    running_thread->state = THREAD_SUSPENDED;

    // reschedule, because we suspended the running thread it
    // should make the scheduler save our state and choose another thread
    schedule(regs, 0);
cleanup:
    if(IS_ERROR(err)) {
        regs->rax = (uint64_t) -err;
        if(provider_thread != NULL) thread_kill(provider_thread);
        if(stack != NULL) mm_free(&kernel_memory_manager, stack);
    }

    if(scheme != NULL) {
        mm_free(&kernel_memory_manager, scheme);
    }

    return err;
}

error_t resource_manager_init() {
    syscalls[SYSCALL_PROVIDER_HANDLER_FINISHED] = syscall_provider_handler_finished;

    syscalls[SYSCALL_OPEN] = dispatch_resource_call;
    syscalls[SYSCALL_CLOSE] = dispatch_resource_call;
    syscalls[SYSCALL_READ] = dispatch_resource_call;
    syscalls[SYSCALL_WRITE] = dispatch_resource_call;
    syscalls[SYSCALL_SELECT] = NULL; // TODO
    syscalls[SYSCALL_POLL] = dispatch_resource_call;
    syscalls[SYSCALL_WAIT] = NULL; // TODO

    return NO_ERROR;
}

error_t resource_manager_register_provider(resource_provider_t* provider) {
    term_print("[resource_manager_register] registered provider for '%s' (pid=%d)\n", provider->scheme, provider->pid);
    buf_push(providers, provider);
    return NO_ERROR;
}

error_t resource_manager_get_provider_by_resource(process_t* process, resource_t resource, resource_provider_t** provider) {
    error_t err = NO_ERROR;
    resource_provider_t* found_provider = NULL;
    int i = 0;

    // Check argiments
    CHECK_ERROR(process != NULL, ERROR_INVALID_ARGUMENT);

    // Search for the resource of the same value and get it's corresponding provider
    for(resource_t* it = process->resources; it < buf_end(process->resources); it++, i++) {
        if(*it != 0 && *it == resource) {
            found_provider = process->providers[i];
            break;
        }
    }

    // Throw error if not found
    CHECK_ERROR(found_provider != NULL, ERROR_NOT_FOUND);
    if(provider != NULL) *provider = found_provider;

    // Set the output parameter
    *provider = found_provider;

cleanup:
    return err;
}

error_t resource_manager_get_provider_by_scheme(const char* scheme, resource_provider_t** provider) {
    error_t err = NO_ERROR;
    resource_provider_t* found_provider = NULL;

    for(resource_provider_t** it = providers; it < buf_end(providers); it++) {
        if(strcmp((*it)->scheme, scheme) == 0) {
            found_provider = *it;
            break;
        }
    }

    CHECK_ERROR(found_provider != NULL, ERROR_NOT_FOUND);
    if(provider != NULL) *provider = found_provider;

cleanup:
    return err;
}

error_t resource_manager_get_provider_by_pid(int pid, resource_provider_t** provider) {
    error_t err = NO_ERROR;
    resource_provider_t* found_provider = NULL;

    for(resource_provider_t** it = providers; it < buf_end(providers); it++) {
        if((*it)->pid == pid) {
            found_provider = *it;
            break;
        }
    }

    CHECK_ERROR(found_provider != NULL, ERROR_NOT_FOUND);
    if(provider != NULL) *provider = found_provider;

cleanup:
    return err;
}
