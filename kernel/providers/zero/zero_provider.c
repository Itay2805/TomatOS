//
// Created by Itay on 07/05/2019.
//

#include <resource/resource_manager.h>

#include <process/scheduler.h>
#include <process/process.h>

#include <graphics/term.h>
#include <common/common.h>
#include <kernel.h>
#include <common/klib.h>

#include "zero_provider.h"

////////////////////////////////////////////////////////////////////////////
// The process functions
////////////////////////////////////////////////////////////////////////////

static resource_provider_t zero_provider = {0};

static error_t handle_close(process_t* process, thread_t* thread, resource_t resource);

static error_t handle_open(process_t* process, thread_t* thread, resource_descriptor_t* descriptor, resource_t* resource) {
    error_t err = NO_ERROR;
    resource_t created_resource = 0;

    UNUSED(descriptor);

    // create the resource
    CHECK_AND_RETHROW(resource_create(process, &zero_provider, &created_resource));

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

static error_t handle_read(process_t* process, thread_t* thread, resource_t resource, char* buffer, size_t len, size_t* read_size) {
    error_t err = NO_ERROR;
    UNUSED(resource);

    CHECK_AND_RETHROW(vmm_clear_user(process->address_space, buffer, len));
    if(read_size != NULL) {
        CHECK_AND_RETHROW(vmm_clear_user(process->address_space, read_size, len));
    }

cleanup:
    return NO_ERROR;
}

static error_t handle_write(process_t* process, thread_t* thread, resource_t resource, const char* buffer, size_t len, size_t* write_size) {
    error_t err = NO_ERROR;

    if(write_size != NULL) {
        CHECK_AND_RETHROW(vmm_clear_user(process->address_space, write_size, len));
    }

cleanup:
    return NO_ERROR;
}

static error_t handle_tell(process_t* process, thread_t* thread, resource_t resource, size_t* pos) {
    error_t err = NO_ERROR;
    size_t zero = 0;

    CHECK_AND_RETHROW(vmm_copy_to_user(process->address_space, &zero, pos, sizeof(size_t)));

cleanup:
    return err;
}

static error_t handle_seek(process_t* process, thread_t* thread, resource_t resource, int type, ptrdiff_t pos) {
    // Does nothing
    return NO_ERROR;
}

////////////////////////////////////////////////////////////////////////////
// Kernel initialization
////////////////////////////////////////////////////////////////////////////

error_t zero_provider_init() {
    error_t err = NO_ERROR;
    process_t* process = process_create(NULL, true);
    thread_kill(process->threads[0]);

    zero_provider.scheme = "zero";
    zero_provider.pid = process->pid;
    zero_provider.open = handle_open;
    zero_provider.close = handle_close;
    zero_provider.read = handle_read;
    zero_provider.write = handle_write;
    zero_provider.tell = handle_tell;
    zero_provider.seek = handle_seek;

    CHECK_AND_RETHROW(resource_manager_register_provider(&zero_provider));

cleanup:
    return err;
}
