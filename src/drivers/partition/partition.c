#include <string.h>
#include <drivers/filesystem/echfs/echfs.h>
#include <memory/mm.h>
#include <util/defs.h>
#include "partition.h"

error_t partition_read(partition_t* self, uint64_t offset, void* buffer, size_t length) {
    error_t err = NO_ERROR;

    uintptr_t lba_high =  self->lba_start + ALIGN_UP(offset + length,self->parent->block_size) / self->parent->block_size;
    CHECK_ERROR(lba_high <= self->lba_end, ERROR_INVALID_PARAMETER);
    CHECK_AND_RETHROW(storage_read(self->parent, self->lba_start * self->parent->block_size + offset, buffer, length));

cleanup:
    return err;
}

error_t partition_mount(partition_t* self) {
    error_t err = NO_ERROR;

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

error_t partition_unmount(partition_t* self) {
    error_t err = NO_ERROR;

    acquire_lock(&self->mount_lock);
    CHECK(self->filesystem != NULL);

    CHECK_AND_RETHROW(self->filesystem->dtor(self->filesystem));
    mm_free_pool(self->filesystem);
    self->filesystem = NULL;

cleanup:
    release_lock(&self->mount_lock);
    return err;
}

