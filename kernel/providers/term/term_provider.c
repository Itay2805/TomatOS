//
// Created by Itay on 08/05/2019.
//

#include <process/process.h>
#include <common/string.h>
#include <process/syscalls.h>

#include "term_provider.h"

////////////////////////////////////////////////////////////////////////////
// The process functions
////////////////////////////////////////////////////////////////////////////

static resource_provider_t term_provider = {0};

static void start() {
    // Kill self
    asm("int $0x80" : : "a"(SYSCALL_THREAD_KILL), "D"(0));
    while(true);
}

static error_t handle_open(process_t* process, int tid, resource_descriptor_t* descriptor, resource_t* resource) {
    error_t err = NO_ERROR;
    resource_t created_resource = 0;

    UNUSED(tid);
    UNUSED(descriptor);

    // create the resource
    CHECK_AND_RETHROW(resource_create(process, &term_provider, &created_resource));

    // copy the resource to user space
    CHECK_AND_RETHROW(vmm_copy_to_user(process->address_space, &created_resource, resource, sizeof(resource_t)));

    cleanup:
    if(IS_ERROR(err) != NO_ERROR) {
        // TODO: Close the resource cause we had an error
    }
    return err;
}

static error_t handle_close(process_t* process, int tid, resource_t resource) {
    // TODO: Add a remove resource function, memory leaking until that is fixed

    UNUSED(process);
    UNUSED(tid);
    UNUSED(resource);

    return NO_ERROR;
}

static error_t handle_write(process_t* process, int tid, resource_t resource, char* buffer, size_t len) {
    error_t err = NO_ERROR;
    char* kbuffer = NULL;

    UNUSED(tid);
    UNUSED(resource);

    kbuffer = mm_allocate(&kernel_memory_manager, len + 1);
    kbuffer[len] = 0;
    CHECK_AND_RETHROW(vmm_copy_to_kernel(process->address_space, buffer, kbuffer, len));

    term_write(kbuffer);

    // Does nothing
cleanup:
    mm_free(&kernel_memory_manager, kbuffer);
    return err;
}

////////////////////////////////////////////////////////////////////////////
// Kernel initialization
////////////////////////////////////////////////////////////////////////////

error_t term_provider_init() {
    process_t* process = process_create(start, true);
    char* stack = NULL;

    stack = mm_allocate(&kernel_memory_manager, 256);

    process->threads[0].cpu_state.rsp = (uint64_t) (stack + 256);
    process->threads[0].cpu_state.rbp = (uint64_t) (stack + 256);

    term_provider.scheme = "term";
    term_provider.pid = process->pid;
    term_provider.open = handle_open;
    term_provider.close = handle_close;
    term_provider.write = handle_write;

    resource_manager_register_provider(&term_provider);

    mm_free(&kernel_memory_manager, stack);
    return NO_ERROR;
}
