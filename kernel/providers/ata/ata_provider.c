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
#include <common/logging.h>
#include <drivers/pci.h>

#include "ata_provider.h"
#include "ata.h"

////////////////////////////////////////////////////////////////////////////
// The process functions
////////////////////////////////////////////////////////////////////////////

typedef struct ata_entry {
    // is this a valid entry
    bool present;

    // the channel (primary vs secondary)
    int channel;

    // the internal bus, 2 per channel
    int bus;

    size_t size_in_bytes;
    spinlock_t lock;
} ata_entry_t;

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

static error_t handle_close(process_t* process, thread_t* thread, resource_t resource);

static error_t handle_open(process_t* process, thread_t* thread, resource_descriptor_t* descriptor, resource_t* resource) {
    error_t err = NO_ERROR;
    resource_t created_resource = 0;
    resource_descriptor_t kdesc;
    char* domain_str = NULL;
    size_t domain_len = 0;
    int controller = 0;

    // we need the resource descriptor and the domain
    CHECK_AND_RETHROW(vmm_copy_to_kernel(process->address_space, descriptor, &kdesc, sizeof(resource_descriptor_t)));
    CHECK_AND_RETHROW(vmm_copy_string_to_kernel(process->address_space, kdesc.domain, NULL, &domain_len));
    domain_str = kalloc(domain_len);
    CHECK_AND_RETHROW(vmm_copy_string_to_kernel(process->address_space, kdesc.domain, domain_str, &domain_len));

    // get the controller
    if(strcmp(domain_str, "primary") == 0) {
        controller = 0;
    }else if(strcmp(domain_str, "secondary") == 0) {
        controller = 1;
    }else {
        term_print("[ata::open] %s\n", domain_str);
        CHECK_FAIL_ERROR(ERROR_INVALID_DOMAIN);
    }

    // check the port
    CHECK_ERROR(kdesc.port >= 0 && kdesc.port < 2, ERROR_INVALID_PORT);

    // get the ide entry
    ata_entry_t* entry = find_entry(controller, kdesc.port);
    CHECK_ERROR(entry && entry->present, ERROR_NOT_FOUND);

    // create the resource and add the context to it
    CHECK_AND_RETHROW(resource_create(process, &ata_provider, &created_resource));
    ata_resource_context_t* context = kalloc(sizeof(ata_resource_context_t));
    context->entry = entry;
    context->ptr = 0;
    map_put_from_uint64(&resource_context_map, hash_resource(process->pid, created_resource), context);
    CHECK_AND_RETHROW(vmm_copy_to_user(process->address_space, &created_resource, resource, sizeof(resource_t)));

cleanup:
    if(IS_ERROR(err)) {
        handle_close(process, thread, created_resource);
    }
    if(domain_str != NULL) {
        kfree(domain_str);
    }
    return err;
}

static error_t handle_close(process_t* process, thread_t* thread, resource_t resource) {
    error_t err = NO_ERROR;

    // get the context
    ata_resource_context_t* context = map_get_from_uint64(&resource_context_map, hash_resource(process->pid, resource));
    CHECK_ERROR(context != NULL, ERROR_NOT_FOUND);

    // remove the context
    kfree(context);
    map_put_from_uint64(&resource_context_map, hash_resource(process->pid, resource), NULL);

    // Remove the resource from the kernel
    CHECK_AND_RETHROW(resource_remove(process, resource));


cleanup:
    return NO_ERROR;
}

static error_t handle_read(process_t* process, thread_t* thread, resource_t resource, char* buffer, size_t len, size_t* read_size) {
    error_t err = NO_ERROR;
    ata_resource_context_t* context = NULL;
    char* kbuffer = NULL;

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
    kbuffer = kalloc(start_padding + len + end_padding);

    // lock the disk and read from it
    spinlock_lock(&context->entry->lock);
    for(int i = 0; i < upper_lba - lower_lba; i++) {
        ata_read_sector(context->entry->channel, context->entry->bus, i + lower_lba, kbuffer + i * 512);
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
        kfree(kbuffer);
    }
    return NO_ERROR;
}

static error_t handle_write(process_t* process, thread_t* thread, resource_t resource, const char* buffer, size_t len, size_t* write_size) {
    error_t err = NO_ERROR;
    ata_resource_context_t* context = NULL;
    char* kbuffer = NULL;

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
    kbuffer = kalloc(start_padding + len + end_padding);
    if(start_padding != 0) ata_read_sector(context->entry->channel, context->entry->bus, lower_lba, kbuffer);
    if(end_padding != 0 && lower_lba != upper_lba - 1) ata_read_sector(context->entry->channel, context->entry->bus, upper_lba - 1, kbuffer + start_padding + len);

    // read the buffer from the user
    CHECK_AND_RETHROW(vmm_copy_to_kernel(process->address_space, buffer, kbuffer + start_padding, len));

    // write it all to the disk
    spinlock_lock(&context->entry->lock);
    for(int i = 0; i < upper_lba - lower_lba; i++) {
        ata_write_sector(context->entry->channel, context->entry->bus, i + lower_lba, kbuffer + i * 512);
    }
    spinlock_unlock(&context->entry->lock);

    // write out the size we wrote
    if(write_size != 0) CHECK_AND_RETHROW(vmm_copy_to_user(process->address_space, &len, write_size, sizeof(size_t)));

    // update the write pointers
    context->ptr += len;

cleanup:
    // don't forget to free the buffer
    if(kbuffer != NULL) {
        kfree(kbuffer);
    }
    return NO_ERROR;
}

static error_t handle_tell(process_t* process, thread_t* thread, resource_t resource, size_t* pos) {
    error_t err = NO_ERROR;
    ata_resource_context_t* context = map_get_from_uint64(&resource_context_map, hash_resource(process->pid, resource));
    CHECK_ERROR(context != NULL, ERROR_INVALID_RESOURCE);

    CHECK_ERROR(pos != NULL, ERROR_INVALID_ARGUMENT);

    // get the context
    context = map_get_from_uint64(&resource_context_map, hash_resource(process->pid, resource));
    CHECK_ERROR(context != NULL, ERROR_INVALID_RESOURCE);

    CHECK_AND_RETHROW(vmm_copy_to_user(process->address_space, &context->ptr, pos, sizeof(size_t)));

cleanup:
    return err;
}

static error_t handle_seek(process_t* process, thread_t* thread, resource_t resource, int type, ptrdiff_t pos) {
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

    LOG_NOTICE("found drive in %s:", drive);

    memcpy(buffer, identify->model_number, sizeof(identify->model_number));
    buffer[sizeof(identify->model_number)] = 0;
    LOG_NOTICE("\tmodel=%s", buffer);

    memcpy(buffer, identify->firmware_revision, sizeof(identify->firmware_revision));
    buffer[sizeof(identify->firmware_revision)] = 0;
    LOG_NOTICE("\trev=%s", buffer);

    memcpy(buffer, identify->serial_number, sizeof(identify->serial_number));
    buffer[sizeof(identify->serial_number)] = 0;
    LOG_NOTICE("\tserial=%s", buffer);

    size_t cap = identify->current_capacity_in_sectors * identify->buffer_size * 512;
    if(cap / GB(1) > 0) {
        LOG_NOTICE("\tcapacity=%dGB (%d sectors)", (int) (cap / GB(1)), identify->current_capacity_in_sectors);
    }else if(cap / MB(1) > 0) {
        LOG_NOTICE("\tcapacity=%dMB (%d sectors)", (int) (cap / MB(1)), identify->current_capacity_in_sectors);
    }else if(cap / KB(1) > 0) {
        LOG_NOTICE("\tcapacity=%dKB (%d sectors)", (int) (cap / KB(1)), identify->current_capacity_in_sectors);
    }else {
        LOG_NOTICE("\tcapacity=%dB (%d sectors)", (int) cap, identify->current_capacity_in_sectors);
    }
}

error_t ata_provider_init() {
    error_t err = NO_ERROR;

    // check for drives
    ata_identify_t identify;

    pci_device_t* ide_device = NULL;
    for(pci_device_t* it = pci_devices; it < buf_end(pci_devices); it++) {
        if(it->class == 0x01 && it->subclass == 0x01) {
            if(ide_device != NULL) {
                LOG_WARN("Detected another IDE controller, currently we only support having one ide controller!");
            }else {
                LOG_INFO("Found IDE controller!");
                int count = 0;

                ide_device = it;
                int bar0 = pci_read_uint32(it, PCI_REG_BAR0);
                int bar1 = pci_read_uint32(it, PCI_REG_BAR1);
                int bar2 = pci_read_uint32(it, PCI_REG_BAR2);
                int bar3 = pci_read_uint32(it, PCI_REG_BAR3);

                LOG_DEBUG("%s TODO: Support DMA", __FILENAME__);
                // int bar4 = pci_read_uint32(it, PCI_REG_BAR4);

                // primary channel
                if((bar0 == 0x0 || bar0 == 0x1) && (bar1 == 0x0 || bar1 == 0x1)) {
                    count++;

                    err = ata_identify(0, 0, &identify);
                    if(!IS_ERROR(err)) {
                        print_identify("ata://primary:0/", &identify);
                        ide_entries[0].present = true;
                        ide_entries[0].channel = 0;
                        ide_entries[0].bus = 0;
                        ide_entries[0].size_in_bytes = identify.current_capacity_in_sectors * 512;
                    }else {
                        if(IS_ERROR(err) != ERROR_NOT_FOUND) {
                            KERNEL_STACK_TRACE(err);
                        }
                        ERROR_FREE(err);
                        LOG_WARN("ata://primary:0/ not found");
                    }

                    err = ata_identify(0, 1, &identify);
                    if(!IS_ERROR(err)) {
                        print_identify("ata://primary:1/", &identify);
                        ide_entries[1].present = true;
                        ide_entries[1].channel = 0;
                        ide_entries[1].bus = 1;
                        ide_entries[1].size_in_bytes = identify.current_capacity_in_sectors * 512;
                    }else {
                        if(IS_ERROR(err) != ERROR_NOT_FOUND) {
                            KERNEL_STACK_TRACE(err);
                        }
                        ERROR_FREE(err);
                        LOG_WARN("ata://primary:1/ not found");
                    }
                }else {
                    LOG_WARN("Primary channel uses incompatible ports");
                }

                // secondary channel
                if((bar2 == 0x0 || bar2 == 0x1) && (bar3 == 0x0 || bar3 == 0x1)) {
                    count++;

                    err = ata_identify(1, 1, &identify);
                    if(!IS_ERROR(err)) {
                        print_identify("ata://secondary:0/", &identify);
                        ide_entries[2].present = true;
                        ide_entries[2].channel = 1;
                        ide_entries[2].bus = 0;
                        ide_entries[2].size_in_bytes = identify.current_capacity_in_sectors * 512;
                    }else {
                        if(IS_ERROR(err) != ERROR_NOT_FOUND) {
                            KERNEL_STACK_TRACE(err);
                        }
                        ERROR_FREE(err);
                        LOG_WARN("ata://secondary:0/ not found");
                    }

                    err = ata_identify(1, 1, &identify);
                    if(!IS_ERROR(err)) {
                        print_identify("ata://secondary:1/", &identify);
                        ide_entries[3].present = true;
                        ide_entries[3].channel = 1;
                        ide_entries[3].bus = 1;
                        ide_entries[3].size_in_bytes = identify.current_capacity_in_sectors * 512;
                    }else {
                        if(IS_ERROR(err) != ERROR_NOT_FOUND) {
                            KERNEL_STACK_TRACE(err);
                        }
                        ERROR_FREE(err);
                        LOG_WARN("ata://secondary:1/ not found");
                    }
                    err = NO_ERROR;
                }else {
                    LOG_WARN("Secondary channel uses incompatible ports");
                }

                if(count == 0) {
                    ide_device = NULL;
                    LOG_WARN("No compatible channels, ignoring controller");
                }
            }
        }
    }

    if(pci_devices == NULL) {
        LOG_WARN("No compatible IDE controller found, will not register ata!");
    }else {
        process_t* process = process_create(NULL, true);
        thread_kill(process->threads[0]);

        ata_provider.scheme = "ata";
        ata_provider.pid = process->pid;
        ata_provider.open = handle_open;
        ata_provider.close = handle_close;
        ata_provider.read = handle_read;
        ata_provider.write = handle_write;
        ata_provider.tell = handle_tell;
        ata_provider.seek = handle_seek;

        CHECK_AND_RETHROW(resource_manager_register_provider(&ata_provider));
    }

cleanup:
    return err;
}
