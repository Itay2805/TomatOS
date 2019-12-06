#include <drivers/partition/gpt/gpt.h>
#include <drivers/partition/partition.h>
#include <memory/mm.h>
#include "storage_object.h"

lock_t storage_objects_lock = 0;
list_entry_t storage_objects = INIT_LIST_ENTRY(storage_objects);

error_t storage_mount(storage_device_t* self) {
    error_t err = NO_ERROR;

    acquire_lock(&self->mount_lock);
    CHECK(is_list_empty(&self->partitions));

    // check if gpt
    if(check_gpt(self)) {
        CHECK_AND_RETHROW(gpt_parse(self));

    // no partition scheme, mount as a single partition
    }else {
        partition_t* part = mm_allocate_pool(sizeof(partition_t));
        part->device.type = DEVICE_PARTITION;
        part->parent = self;
        part->lba_start = 0;
        part->lba_end = self->lba_limit;

        insert_tail_list(&self->partitions, &part->device.link);

        debug_log("[+] storage: Added a raw partition `%s`\n", self->device.name);
    }

cleanup:
    release_lock(&self->mount_lock);
    return NO_ERROR;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Storage Device functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// TODO: Add caching at this level?
error_t storage_read(storage_device_t* self, uintptr_t offset, void* buffer, size_t length) {
    error_t err = NO_ERROR;

    CHECK_ERROR(self->lba_limit * self->block_size >= offset + length, ERROR_OUT_OF_RESOURCE);
    CHECK_AND_RETHROW(self->read(self, offset, buffer, length));

cleanup:
    return err;
}

