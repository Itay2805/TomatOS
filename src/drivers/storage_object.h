#ifndef TOMATKERNEL_STORAGE_OBJECT_H
#define TOMATKERNEL_STORAGE_OBJECT_H

#include <objects/object.h>
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

typedef struct storage_device_class {
    class_t super;

    size_t (*get_block_count)(void* self);
    size_t (*get_block_size)(void* self);
    error_t (*read_block)(void* self, uintptr_t lba, void* buffer);
} storage_device_class_t;

typedef struct storage_device {
    object_t super;

    // link to the rest of entries
    list_entry_t link;

    // the partitions mounted on this device
    list_entry_t partitions;
} storage_device_t;

/**
 * The storage device class, this has all the functions
 * needed for normal operation of the device
 */
const void* StorageDeviceClass();

/**
 * Generic Storage Device class handles everything which is going to
 * be common to every storage device
 */
const void* StorageDevice();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Storage Device functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Get the amount of blocks that are on the device
 *
 * attempting to read more than this will result in an error
 */
size_t storage_get_block_count(void* _self);

/**
 * Get the block size of the storage device
 *
 * Needed for calculating the lba when reading
 */
size_t storage_get_block_size(void* _self);

// TODO: allow for reading multiple sectors at a time
error_t storage_read_block(void* _self, uintptr_t lba, void* buffer);

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
error_t storage_mount(void* _self);

/**
 * Unmount the storage device, this will delete all the partitions mounted
 * on this storage device
 */
error_t storage_unmount(void* _self);

#endif //TOMATKERNEL_STORAGE_OBJECT_H
