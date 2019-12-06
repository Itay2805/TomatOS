#include <drivers/storage/storage_object.h>
#include <tboot.h>
#include <string.h>
#include <memory/mm.h>
#include "initrd.h"

typedef struct initrd_device {
    storage_device_t super;

    char* base;
} initrd_device_t;

static error_t initrd_read(initrd_device_t* self, uintptr_t offset, void* buffer, size_t len) {
    error_t err = NO_ERROR;

    CHECK_ERROR(self != NULL, ERROR_INVALID_PARAMETER);
    CHECK_ERROR(len > 0, ERROR_INVALID_PARAMETER);
    CHECK_ERROR(offset + len < self->super.lba_limit, ERROR_INVALID_PARAMETER);
    CHECK_ERROR(buffer != NULL, ERROR_INVALID_PARAMETER);

    memcpy(buffer, self->base + offset, len);

cleanup:
    return err;
}

void initrd_init(tboot_info_t* info) {
    // simply iterate all the modules and find all the ones named "initrd"
    for(int i = 0; i < info->modules.count; i++) {
        tboot_module_t* module = &info->modules.entries[i];
        if(strcmp(module->name, "initrd") == 0) {
            debug_log("[*] Found initrd\n");

            initrd_device_t* device = mm_allocate_pool(sizeof(initrd_device_t));

            // setup the device
            device->super.device.type = DEVICE_STORAGE;
            strcpy(device->super.device.name, "initrd");

            // set up the storage device
            device->super.block_size = 1;
            device->super.lba_limit = module->len;
            device->super.read = (void*)initrd_read;

            // setup self
            device->base = (char*)module->base;

            // add it
            acquire_lock(&storage_objects_lock);
            insert_tail_list(&storage_objects, &device->super.device.link);
            release_lock(&storage_objects_lock);
        }
    }
}
