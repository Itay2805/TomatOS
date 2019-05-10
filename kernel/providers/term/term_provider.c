//
// Created by Itay on 08/05/2019.
//

#include <process/process.h>
#include <common/string.h>
#include <process/syscalls.h>
#include <common/klib.h>

#include "term_provider.h"

////////////////////////////////////////////////////////////////////////////
// The process functions
////////////////////////////////////////////////////////////////////////////

static resource_provider_t term_provider = {0};

static error_t handle_close(process_t* process, thread_t* thread, resource_t resource);

static error_t handle_open(process_t* process, thread_t* thread, resource_descriptor_t* descriptor, resource_t* resource) {
    error_t err = NO_ERROR;
    resource_t created_resource = 0;

    // create the resource
    CHECK_AND_RETHROW(resource_create(process, &term_provider, &created_resource));

    // copy the resource to user space
    CHECK_AND_RETHROW(vmm_copy_to_user(process->address_space, &created_resource, resource, sizeof(resource_t)));

cleanup:
    if(IS_ERROR(err)) {
        handle_close(process, thread, created_resource);
    }
    return err;
}

static error_t handle_close(process_t* process, thread_t* thread, resource_t resource) {
    error_t err = NO_ERROR;

    CHECK_AND_RETHROW(resource_remove(process, resource));

cleanup:
    return NO_ERROR;
}

static error_t handle_write(process_t* process, thread_t* thread, resource_t resource, char* buffer, size_t len) {
    error_t err = NO_ERROR;
    char* kbuffer = NULL;

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
    error_t err = NO_ERROR;

    process_t* process = process_create(NULL, true);
    thread_kill(process->threads[0]);

    term_provider.scheme = "term";
    term_provider.pid = process->pid;
    term_provider.open = handle_open;
    term_provider.close = handle_close;
    term_provider.write = handle_write;

    CHECK_AND_RETHROW(resource_manager_register_provider(&term_provider));

cleanup:
    return err;
}
