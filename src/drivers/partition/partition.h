#ifndef TOMATKERNEL_PARTITION_H
#define TOMATKERNEL_PARTITION_H

#include <util/list.h>
#include <drivers/storage/storage_object.h>

struct filesystem;

typedef struct partition {
    device_t device;

    // the parent of the partition
    storage_device_t* parent;

    // the limits of the partition
    uint64_t lba_start;
    uint64_t lba_end;

    // mount look
    lock_t mount_lock;
    struct filesystem* filesystem;
} partition_t;

/**
 * Read from a partition
 */
error_t partition_read(partition_t* self, uint64_t offset, void* buffer, size_t length);

/**
 * Attempt to mount the partition into a filesystem
 */
error_t partition_mount(partition_t* self);

/**
 * Unmount the mounted filesystem
 */
error_t partition_unmount(partition_t* self);

#endif //TOMATKERNEL_PARTITION_H
