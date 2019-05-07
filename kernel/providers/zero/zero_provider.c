//
// Created by Itay on 07/05/2019.
//

#include <resource/resource_manager.h>

#include <process/scheduler.h>
#include <process/process.h>

#include <graphics/term.h>
#include <common/common.h>

#include "zero_provider.h"

////////////////////////////////////////////////////////////////////////////
// The process functions
////////////////////////////////////////////////////////////////////////////

static resource_provider_t zero_provider = {0};

static void start() {
    // Nothing to initialize
}

static error_t handle_open(int pid, int tid, resource_descriptor_t* descriptor, resource_t* resource) {
    error_t err = NO_ERROR;

    // TODO: how can we handle the problem that there might be a context switch which could cause another process to be created
    //       while we have this pointer...
    process_t* process = NULL;

    CHECK_AND_RETHROW(process_find(pid, &process));
    CHECK_AND_RETHROW(resource_create(process, &zero_provider, resource));

cleanup:
    return err;
}

static error_t handle_close(int pid, int tid, resource_t resource) {
    // TODO: Add a remove resource function, memory leaking until that is fixed
    return NO_ERROR;
}

static error_t handle_read(int pid, int tid, resource_t resource, char* buffer, size_t len) {

    return NO_ERROR;
}

static error_t handle_write(int pid, int tid, resource_t resource, char* buffer, size_t len) {
    vmm_copy
    return NO_ERROR;
}

static error_t handle_tell(ATTR_UNUSED int pid, ATTR_UNUSED int tid, resource_t ATTR_UNUSED resource, size_t* ATTR_UNUSED pos) {
    // TODO: Make so this will put zero in the pos
    return NO_ERROR;
}

static error_t handle_seek(ATTR_UNUSED int pid, ATTR_UNUSED int tid, ATTR_UNUSED resource_t resource, ATTR_UNUSED int type, ATTR_UNUSED size_t* pos) {
    // Does nothing
    return NO_ERROR;
}

////////////////////////////////////////////////////////////////////////////
// Kernel initialization
////////////////////////////////////////////////////////////////////////////

error_t zero_provider_init() {
    process_t* process = process_create(start, true);

    zero_provider.scheme = "zero";
    zero_provider.pid = process->pid;
    zero_provider.open = handle_open;
    zero_provider.close = handle_close;
    zero_provider.read = handle_read;
    zero_provider.write = handle_write;
    zero_provider.tell = handle_tell;
    zero_provider.seek = handle_seek;

    term_write("[zero_provider_init] Registering zero provider");
    return NO_ERROR;
}
