//
// Created by Itay on 07/05/2019.
//

#include <resource/resource_manager.h>

#include <process/scheduler.h>
#include <process/process.h>

#include <graphics/term.h>
#include <common/common.h>
#include <kernel.h>
#include <locks/rwlock.h>
#include <common/klib.h>

#include "ata_provider.h"
#include "context_manager.h"
#include "ata.h"

////////////////////////////////////////////////////////////////////////////
// The process functions
////////////////////////////////////////////////////////////////////////////

static resource_provider_t ata_provider = {0};

ata_entry_t ide_entries[4];

static ata_entry_t* find_entry(int domain, int port) {
    if(domain * 2 + port < 0 || domain * 2 + port >= 4) {
        return NULL;
    }
    return &ide_entries[domain * 2 + port];
}

static void start() {
    // Nothing to initialize, but we do not want to exit the thread so just have a while true,
    // will probably want to kill it tbh
    tkill(0);
    while(true);
}

static error_t handle_open(process_t* process, int tid, resource_descriptor_t* descriptor, resource_t* resource) {
    error_t err = NO_ERROR;
    resource_t created_resource = 0;
    resource_descriptor_t kdesc;
    char* domain_str = NULL;
    size_t domain_len = 0;
    int controller = 0;

    UNUSED(tid);

    // we need the resource descriptor and the domain
    CHECK_AND_RETHROW(vmm_copy_to_kernel(process->address_space, descriptor, &kdesc, sizeof(resource_descriptor_t)));
    CHECK_AND_RETHROW(vmm_copy_string_to_kernel(process->address_space, kdesc.domain, NULL, &domain_len));
    domain_str = mm_allocate(&kernel_memory_manager, domain_len);
    CHECK_AND_RETHROW(vmm_copy_string_to_kernel(process->address_space, kdesc.domain, domain_str, &domain_len));

    // get the controller
    if(strcmp(domain_str, "primary") == 0) {
        controller = 0;
    }else if(strcmp(domain_str, "secondary") == 0) {
        controller = 1;
    }else {
        CHECK_FAIL_ERROR(ERROR_INVALID_DOMAIN);
    }

    // check the port
    CHECK_ERROR(kdesc.port >= 0 && kdesc.port < 2, ERROR_INVALID_PORT);

    // get the ide entry
    ata_entry_t* entry = find_entry(controller, kdesc.port);
    CHECK_ERROR(entry, ERROR_NOT_FOUND);

    // create the resource and add the context to it
    CHECK_AND_RETHROW(resource_create(process, &ata_provider, &created_resource));
    ata_resource_context_t* context = mm_allocate(&kernel_memory_manager, sizeof(ata_resource_context_t));
    context->resource = created_resource;
    context->entry = entry;
    context->ptr = 0;
    context->rwlock = (rwlock_t){0};
    ata_context_add(context);
    CHECK_AND_RETHROW(vmm_copy_to_user(process->address_space, &created_resource, resource, sizeof(resource_t)));

cleanup:
    if(IS_ERROR(err) != NO_ERROR) {
        // TODO: Close the resource cause we had an error
    }
    if(domain_str != NULL) {
        mm_free(&kernel_memory_manager, domain_str);
    }
    return err;
}

static error_t handle_close(process_t* process, int tid, resource_t resource) {
    // TODO: Add a remove resource function, memory leaking until that is fixed
    error_t err = NO_ERROR;

    UNUSED(process);
    UNUSED(tid);

    CHECK_AND_RETHROW(ata_context_remove(resource));

cleanup:
    return NO_ERROR;
}

static error_t handle_read(process_t* process, int tid, resource_t resource, char* buffer, size_t len, size_t* read_size) {
    error_t err = NO_ERROR;
    ata_resource_context_t* context = NULL;

    UNUSED(tid);

    CHECK_AND_RETHROW(ata_context_get(resource, &context));



cleanup:
    return NO_ERROR;
}

static error_t handle_write(process_t* process, int tid, resource_t resource, char* buffer, size_t len, size_t* write_size) {

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

error_t ata_provider_init() {
    process_t* process = process_create(start, true);
    char* stack = NULL;

    stack = mm_allocate(&kernel_memory_manager, 256);

    process->threads[0].cpu_state.rsp = (uint64_t) (stack + 256);
    process->threads[0].cpu_state.rbp = (uint64_t) (stack + 256);

    ata_provider.scheme = "ata";
    ata_provider.pid = process->pid;
    ata_provider.open = handle_open;
    ata_provider.close = handle_close;
    ata_provider.read = handle_read;
    ata_provider.write = handle_write;
    ata_provider.tell = handle_tell;
    ata_provider.seek = handle_seek;

    resource_manager_register_provider(&ata_provider);

    // check for drives
    ata_identify_t identify;

    // master:0
    if(ata_identify(0, 0, &identify)) {
        term_print("[ata_provider_init] found drive in ata://primary:0/ (model=%s, rev=%s)\n", identify.model_number, identify.firmware_revision);
        ide_entries[0].present = true;
        ide_entries[0].controller = 0;
        ide_entries[0].port = 1;
    }else {
        term_print("[ata_provider_init] ata://primary:0/ is empty\n");
    }

    if(ata_identify(0, 1, &identify)) {
        term_print("[ata_provider_init] found drive in ata://primary:1/ (model=%s, rev=%s)\n", identify.model_number, identify.firmware_revision);
        ide_entries[1].present = true;
        ide_entries[1].controller = 0;
        ide_entries[1].port = 1;
    }else {
        term_print("[ata_provider_init] ata://primary:1/ is empty\n");
    }

    if(ata_identify(1, 0, &identify)) {
        term_print("[ata_provider_init] found drive in ata://secondary:0/ (model=%s, rev=%s)\n", identify.model_number, identify.firmware_revision);
        ide_entries[2].present = true;
        ide_entries[2].controller = 0;
        ide_entries[2].port = 1;
    }else {
        term_print("[ata_provider_init] ata://secondary:0/ is empty\n");
    }

    if(ata_identify(1, 1, &identify)) {
        term_print("[ata_provider_init] found drive in ata://secondary:1/ (model=%s, rev=%s)\n", identify.model_number, identify.firmware_revision);
        ide_entries[3].present = true;
        ide_entries[3].controller = 0;
        ide_entries[3].port = 1;
    }else {
        term_print("[ata_provider_init] ata://secondary:1/ is empty\n");
    }

    mm_free(&kernel_memory_manager, stack);
    return NO_ERROR;
}
