//
// Created by Itay on 07/05/2019.
//

#include <resource/resource_manager.h>

#include <process/scheduler.h>
#include <process/process.h>

#include <graphics/term.h>
#include <common/common.h>
#include <common/klib.h>
#include <kernel.h>
#include <common/map.h>

#include "ata_provider.h"
#include "ata.h"

////////////////////////////////////////////////////////////////////////////
// The process functions
////////////////////////////////////////////////////////////////////////////

typedef struct ata_resource_context {
    ata_entry_t* entry;
    size_t ptr;
} ata_resource_context_t;

map_t resource_context_map = {0};

static resource_provider_t ata_provider = {0};

ata_entry_t ide_entries[4];

static ata_entry_t* find_entry(int domain, int port) {
    if(domain * 2 + port < 0 || domain * 2 + port >= 4) {
        return NULL;
    }
    return &ide_entries[domain * 2 + port];
}

static error_t handle_close(process_t* process, int tid, resource_t resource);

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
    context->entry = entry;
    context->ptr = 0;
    map_put_from_uint64(&resource_context_map, hash_resource(process->pid, created_resource), context);
    CHECK_AND_RETHROW(vmm_copy_to_user(process->address_space, &created_resource, resource, sizeof(resource_t)));

cleanup:
    if(IS_ERROR(err)) {
        handle_close(process, tid, created_resource);
    }
    if(domain_str != NULL) {
        mm_free(&kernel_memory_manager, domain_str);
    }
    return err;
}

static error_t handle_close(process_t* process, int tid, resource_t resource) {
    error_t err = NO_ERROR;

    // get the context
    ata_resource_context_t* context = map_get_from_uint64(&resource_context_map, hash_resource(process->pid, resource));
    CHECK_ERROR(context != NULL, ERROR_NOT_FOUND);

    // remove the context
    mm_free(&kernel_memory_manager, context);
    map_put_from_uint64(&resource_context_map, hash_resource(process->pid, resource), NULL);

    // Remove the resource from the kernel
    CHECK_AND_RETHROW(resource_remove(process, resource));


cleanup:
    return NO_ERROR;
}

static error_t handle_read(process_t* process, int tid, resource_t resource, char* buffer, size_t len, size_t* read_size) {
    error_t err = NO_ERROR;
    ata_resource_context_t* context = NULL;
    char* kbuffer = NULL;
    size_t read = 0;
    UNUSED(tid);

    CHECK_ERROR(buffer != NULL, ERROR_INVALID_ARGUMENT);
    CHECK_ERROR(len > 0, ERROR_INVALID_ARGUMENT);

    context = map_get_from_uint64(&resource_context_map, hash_resource(process->pid, resource));
    CHECK_ERROR(context != NULL, ERROR_INVALID_RESOURCE);

    // calculate how much can actually read
    len = MIN(context->entry->size_in_bytes - context->ptr, len);
    CHECK_ERROR(len > 0, ERROR_OUT_OF_RANGE);

    // calculate the padding and allocate a buffer
    size_t lower_lba = (size_t) ((ALIGN_DOWN(context->ptr, 512)) / 512);
    size_t upper_lba = (size_t) ((ALIGN_UP(context->ptr + len, 512)) / 512);
    size_t start_padding = context->ptr - lower_lba * 512;
    size_t end_padding = upper_lba * 512 - (context->ptr + len);
    kbuffer = mm_allocate(&kernel_memory_manager, start_padding + len + end_padding);

    // lock the disk and read from it
    spinlock_lock(&context->entry->lock);
    for(int i = 0; i < upper_lba - lower_lba; i++) {
        ata_read_sector(context->entry->controller, context->entry->port, i + lower_lba, kbuffer + i * 512);
    }
    spinlock_unlock(&context->entry->lock);

    // copy the buffer to the user
    CHECK_AND_RETHROW(vmm_copy_to_user(process->address_space, kbuffer + start_padding, buffer, len));
    if(read_size != NULL) CHECK_AND_RETHROW(vmm_copy_to_user(process->address_space, &len, read_size, sizeof(size_t)));

    // update the read pointers
    context->ptr += len;

cleanup:
    // don't forget to free the buffer
    if(kbuffer != NULL) {
        mm_free(&kernel_memory_manager, kbuffer);
    }
    return NO_ERROR;
}

static error_t handle_write(process_t* process, int tid, resource_t resource, char* buffer, size_t len, size_t* write_size) {
    error_t err = NO_ERROR;
    ata_resource_context_t* context = NULL;
    char* kbuffer = NULL;

    UNUSED(tid);

    CHECK_ERROR(buffer != NULL, ERROR_INVALID_ARGUMENT);
    CHECK_ERROR(len > 0, ERROR_INVALID_ARGUMENT);

    // get the context
    context = map_get_from_uint64(&resource_context_map, hash_resource(process->pid, resource));
    CHECK_ERROR(context != NULL, ERROR_INVALID_RESOURCE);

    // calculate how much can actually write
    len = MIN(context->entry->size_in_bytes - context->ptr, len);
    CHECK_ERROR(len > 0, ERROR_OUT_OF_RANGE);

    // calculate padding at start and end
    size_t lower_lba = (size_t) ((ALIGN_DOWN(context->ptr, 512)) / 512);
    size_t upper_lba = (size_t) ((ALIGN_UP(context->ptr + len, 512)) / 512);
    size_t start_padding = context->ptr - lower_lba * 512;
    size_t end_padding = upper_lba * 512 - (start_padding + context->ptr + len);

    // allocate the buffer and read the paddings
    kbuffer = mm_allocate(&kernel_memory_manager, start_padding + len + end_padding);
    if(start_padding != 0) ata_read_sector(context->entry->controller, context->entry->port, lower_lba, kbuffer);
    if(end_padding != 0 && lower_lba != upper_lba - 1) ata_read_sector(context->entry->controller, context->entry->port, upper_lba - 1, kbuffer + start_padding + len);

    // read the buffer from the user
    CHECK_AND_RETHROW(vmm_copy_to_kernel(process->address_space, buffer, kbuffer + start_padding, len));

    // write it all to the disk
    spinlock_lock(&context->entry->lock);
    for(int i = 0; i < upper_lba - lower_lba; i++) {
        ata_write_sector(context->entry->controller, context->entry->port, i + lower_lba, kbuffer + i * 512);
    }
    spinlock_unlock(&context->entry->lock);

    // write out the size we wrote
    if(write_size != 0) CHECK_AND_RETHROW(vmm_copy_to_user(process->address_space, &len, write_size, sizeof(size_t)));

    // update the write pointers
    context->ptr += len;

cleanup:
    // don't forget to free the buffer
    if(kbuffer != NULL) {
        mm_free(&kernel_memory_manager, kbuffer);
    }
    return NO_ERROR;
}

static error_t handle_tell(process_t* process, int tid, resource_t resource, size_t* pos) {
    error_t err = NO_ERROR;
    ata_resource_context_t* context = map_get_from_uint64(&resource_context_map, hash_resource(process->pid, resource));
    CHECK_ERROR(context != NULL, ERROR_INVALID_RESOURCE);

    UNUSED(tid);

    CHECK_ERROR(pos != NULL, ERROR_INVALID_ARGUMENT);

    // get the context
    context = map_get_from_uint64(&resource_context_map, hash_resource(process->pid, resource));
    CHECK_ERROR(context != NULL, ERROR_INVALID_RESOURCE);

    CHECK_AND_RETHROW(vmm_copy_to_user(process->address_space, &context->ptr, pos, sizeof(size_t)));

cleanup:
    return err;
}

static error_t handle_seek(process_t* process, int tid, resource_t resource, int type, ptrdiff_t pos) {
    error_t err = NO_ERROR;
    ata_resource_context_t* context = map_get_from_uint64(&resource_context_map, hash_resource(process->pid, resource));
    CHECK_ERROR(context != NULL, ERROR_INVALID_RESOURCE);

    switch(type) {
        case SEEK_CUR:
            context->ptr += pos;
            break;

        case SEEK_END:
            context->ptr = context->entry->size_in_bytes - pos;
            break;

        case SEEK_START:
            context->ptr = (size_t) pos;
            break;

        default:
            CHECK_FAIL_ERROR(ERROR_NOT_IMPLEMENTED);
    }

    CHECK_ERROR(context->ptr < context->entry->size_in_bytes, ERROR_OUT_OF_RANGE);

cleanup:
    if(IS_ERROR(err)) {
        context->ptr = context->entry->size_in_bytes;
    }
    return err;
}

////////////////////////////////////////////////////////////////////////////
// Kernel initialization
////////////////////////////////////////////////////////////////////////////

static void print_identify(const char* drive, ata_identify_t* identify) {
    char buffer[41];

    term_print("[ata_provider_init] found drive in %s:\n", drive);

    memcpy(buffer, identify->model_number, sizeof(identify->model_number));
    buffer[sizeof(identify->model_number)] = 0;
    term_print("[ata_provider_init] \tmodel=%s\n", buffer);

    memcpy(buffer, identify->firmware_revision, sizeof(identify->firmware_revision));
    buffer[sizeof(identify->firmware_revision)] = 0;
    term_print("[ata_provider_init] \trev=%s\n", buffer);

    memcpy(buffer, identify->serial_number, sizeof(identify->serial_number));
    buffer[sizeof(identify->serial_number)] = 0;
    term_print("[ata_provider_init] \tserial=%s\n", buffer);

    size_t cap = identify->current_capacity_in_sectors * identify->buffer_size * 512;
    if(cap / GB(1) > 0) {
        term_print("[ata_provider_init] \tcapacity=%dGB (%d sectors)\n", (int) (cap / GB(1)), identify->current_capacity_in_sectors);
    }else if(cap / MB(1) > 0) {
        term_print("[ata_provider_init] \tcapacity=%dMB (%d sectors)\n", (int) (cap / MB(1)), identify->current_capacity_in_sectors);
    }else if(cap / KB(1) > 0) {
        term_print("[ata_provider_init] \tcapacity=%dKB (%d sectors)\n", (int) (cap / KB(1)), identify->current_capacity_in_sectors);
    }else {
        term_print("[ata_provider_init] \tcapacity=%dB (%d sectors)\n", (int) cap, identify->current_capacity_in_sectors);
    }
}

error_t ata_provider_init() {
    error_t err = NO_ERROR;

    process_t* process = process_create(NULL, true);
    thread_kill(&process->threads[0]);

    ata_provider.scheme = "ata";
    ata_provider.pid = process->pid;
    ata_provider.open = handle_open;
    ata_provider.close = handle_close;
    ata_provider.read = handle_read;
    ata_provider.write = handle_write;
    ata_provider.tell = handle_tell;
    ata_provider.seek = handle_seek;

    CHECK_AND_RETHROW(resource_manager_register_provider(&ata_provider));

    // check for drives
    ata_identify_t identify;

    // master:0
    if(!IS_ERROR(ata_identify(0, 0, &identify))) {
        print_identify("ata://primary:0/", &identify);
        ide_entries[0].present = true;
        ide_entries[0].controller = 0;
        ide_entries[0].port = 0;
        ide_entries[0].size_in_bytes = identify.current_capacity_in_sectors * 512;
    }else {
        term_print("[ata_provider_init] ata://primary:0/ not found\n");
    }

    if(!IS_ERROR(ata_identify(0, 1, &identify))) {
        print_identify("ata://primary:1/", &identify);
        ide_entries[1].present = true;
        ide_entries[1].controller = 0;
        ide_entries[1].port = 1;
        ide_entries[1].size_in_bytes = identify.current_capacity_in_sectors * 512;
    }else {
        term_print("[ata_provider_init] ata://primary:1/ not found\n");
    }

    if(!IS_ERROR(ata_identify(1, 0, &identify))) {
        print_identify("ata://secondary:0/", &identify);
        ide_entries[2].present = true;
        ide_entries[2].controller = 1;
        ide_entries[2].port = 0;
        ide_entries[2].size_in_bytes = identify.current_capacity_in_sectors * 512;
    }else {
        term_print("[ata_provider_init] ata://secondary:0/ not found\n");
    }

    if(!IS_ERROR(ata_identify(1, 1, &identify))) {
        print_identify("ata://secondary:1/", &identify);
        ide_entries[3].present = true;
        ide_entries[3].controller = 1;
        ide_entries[3].port = 1;
        ide_entries[3].size_in_bytes = identify.current_capacity_in_sectors * 512;
    }else {
        term_print("[ata_provider_init] ata://secondary:1/ not found\n");
    }

cleanup:
    return err;
}
