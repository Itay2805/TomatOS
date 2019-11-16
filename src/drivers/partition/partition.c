#include <string.h>
#include <drivers/filesystem/echfs/echfs.h>
#include "partition.h"

static void* Partition_ctor(void* _self, va_list ap) {
    partition_t* self = super_ctor(Partition(), _self, ap);

    // set the info
    self->parent = va_arg(ap, storage_device_t*);
    self->lba_start = va_arg(ap, uint64_t);
    self->lba_end = va_arg(ap, uint64_t);

    // copy the name
    strncpy(self->name, va_arg(ap, char*), sizeof(self->name) - 1);

    // add to parent
    insert_tail_list(&self->parent->partitions, &self->link);

    return self;
}

static void* Partition_dtor(void* _self) {
    partition_t* self = super_dtor(Partition(), _self);

    partition_unmount(self);

    remove_entry_list(&self->link);

    return self;
}

const void* Partition() {
    static const void* class = NULL;
    if(class == NULL) {
        class = new(Class(),
                "Partition", Object(), sizeof(partition_t),
                ctor, Partition_ctor,
                dtor, Partition_dtor);
    }
    return class;
}

error_t partition_read_block(void* _self, uint64_t lba, void* buffer, size_t size) {
    error_t err = NO_ERROR;
    partition_t* partition = cast(Partition(), _self);

    // make sure in range
    CHECK(partition->lba_start + lba <= partition->lba_end);

    // read it
    CHECK_AND_RETHROW(storage_read_block(partition->parent, partition->lba_start + lba, buffer, size));

cleanup:
    return err;
}

error_t partition_mount(void* _self) {
    error_t err = NO_ERROR;
    partition_t* self = cast(Partition(), _self);

    acquire_lock(&self->mount_lock);
    CHECK(self->filesystem == NULL);

    if(check_echfs(self)) {
        debug_log("[+] Found ECHFS\n");
    }else {
        CHECK_ERROR(false, ERROR_UNSUPPORTED);
    }

cleanup:
    release_lock(&self->mount_lock);
    return err;
}

error_t partition_unmount(void* _self) {
    error_t err = NO_ERROR;
    partition_t* self = cast(Partition(), _self);

    acquire_lock(&self->mount_lock);
    CHECK(self->filesystem != NULL);

    delete(self->filesystem);
    self->filesystem = NULL;

cleanup:
    release_lock(&self->mount_lock);
    return err;
}

