#ifndef TOMATKERNEL_STORAGE_OBJECT_H
#define TOMATKERNEL_STORAGE_OBJECT_H

#include <drivers/device.h>

#include <util/error.h>
#include <util/sync.h>

/**
 * This is used to lock the storage objects list
 */
extern lock_t storage_objects_lock;

/**
 * This is an array with all the storage objects
 */
extern list_entry_t storage_objects;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class and object definitions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct storage_device {
    // the device itself
    device_t device;

    // size of a single block
    size_t block_size;

    // the highest lba supported by the device
    size_t lba_limit;

    // the partitions mounted to the device
    lock_t mount_lock;
    list_entry_t partitions;

    // block reading function
    error_t (*read)(struct storage_device* self, uintptr_t offset, void* buffer, size_t length);
} storage_device_t;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Storage Device functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Read the length of bytes from the offset into the buffer.
 *
 * This does not require block alignment but it can be helpful for
 * better performance.
 *
 * @param _self     [IN] The device to read from
 * @param offset    [IN] The offset to the first byte to read
 * @param buffer    [IN] The buffer to read to
 * @param length    [IN] The amount of bytes to read
 */
error_t storage_read(storage_device_t* _self, uintptr_t offset, void* buffer, size_t length);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Generic storage device functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Attempt to mount a storage device
 *
 * Mounting a storage device means parsing the partition scheme and creating
 * a partition object for it
 *
 * it is generic to all storage devices
 */
error_t storage_mount(storage_device_t* _self);

/**
 * Unmount the storage device, this will delete all the partitions mounted
 * on this storage device
 */
error_t storage_unmount(storage_device_t* _self);

#endif //TOMATKERNEL_STORAGE_OBJECT_H
