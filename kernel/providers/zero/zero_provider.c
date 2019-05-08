//
// Created by Itay on 07/05/2019.
//

#include <resource/resource_manager.h>

#include <process/scheduler.h>
#include <process/process.h>

#include <graphics/term.h>
#include <common/common.h>
#include <kernel.h>

#include "zero_provider.h"

////////////////////////////////////////////////////////////////////////////
// The process functions
////////////////////////////////////////////////////////////////////////////

static resource_provider_t zero_provider = {0};

static void start() {
    // Nothing to initialize, but we do not want to exit the thread so just have a while true,
    // will probably want to kill it tbh
    while(true);
}

static error_t handle_open(process_t* process, int tid, resource_descriptor_t* descriptor, resource_t* resource) {
    error_t err = NO_ERROR;
    resource_t created_resource = 0;

    UNUSED(tid);
    UNUSED(descriptor);

    // create the resource
    CHECK_AND_RETHROW(resource_create(process, &zero_provider, &created_resource));

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

static error_t handle_read(process_t* process, int tid, resource_t resource, char* buffer, size_t len) {
    error_t err = NO_ERROR;

    UNUSED(tid);
    UNUSED(resource);

    CHECK_AND_RETHROW(vmm_clear_user(process->address_space, buffer, len));

cleanup:
    return NO_ERROR;
}

static error_t handle_write(process_t* process, int tid, resource_t resource, char* buffer, size_t len) {

    UNUSED(process);
    UNUSED(tid);
    UNUSED(resource);
    UNUSED(buffer);
    UNUSED(len);

    // Does nothing
    return NO_ERROR;
}

static error_t handle_tell(process_t* process, int tid, resource_t resource, size_t* pos) {
    error_t err = NO_ERROR;
    size_t zero = 0;

    UNUSED(tid);
    UNUSED(resource);

    CHECK_AND_RETHROW(vmm_copy_to_user(process->address_space, &zero, pos, sizeof(size_t)));

cleanup:
    return err;
}

static error_t handle_seek(process_t* process, int tid, resource_t resource, int type, size_t* pos) {
    UNUSED(process);
    UNUSED(tid);
    UNUSED(resource);
    UNUSED(type);
    UNUSED(pos);
    // Does nothing
    return NO_ERROR;
}

////////////////////////////////////////////////////////////////////////////
// Kernel initialization
////////////////////////////////////////////////////////////////////////////

error_t zero_provider_init() {
    process_t* process = process_create(start, true);
    char* stack = NULL;

    stack = mm_allocate(&kernel_memory_manager, 256);

    process->threads[0].cpu_state.rsp = (uint64_t) (stack + 256);
    process->threads[0].cpu_state.rbp = (uint64_t) (stack + 256);

    zero_provider.scheme = "zero";
    zero_provider.pid = process->pid;
    zero_provider.open = handle_open;
    zero_provider.close = handle_close;
    zero_provider.read = handle_read;
    zero_provider.write = handle_write;
    zero_provider.tell = handle_tell;
    zero_provider.seek = handle_seek;

    term_write("[zero_provider_init] Registering zero provider\n");
    resource_manager_register_provider(&zero_provider);

    mm_free(&kernel_memory_manager, stack);
    return NO_ERROR;
}
