#ifndef TOMATKERNEL_PARTITION_H
#define TOMATKERNEL_PARTITION_H

#include <objects/object.h>
#include <util/list.h>
#include <drivers/storage/storage_object.h>

typedef struct partition {
    object_t super;

    char name[255];

    storage_device_t* parent;

    uint64_t lba_start;
    uint64_t lba_end;

    list_entry_t link;

    lock_t mount_lock;
    void* filesystem;
} partition_t;

const void* Partition();

/**
 * Read from a partition
 */
error_t partition_read_block(void* _self, uint64_t lba, void* buffer, size_t size);

/**
 * Attempt to mount the partition into a filesystem
 */
error_t partition_mount(void* _self);

/**
 * Unmount the mounted filesystem
 */
error_t partition_unmount(void* _self);

#endif //TOMATKERNEL_PARTITION_H
