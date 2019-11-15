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

    error_t (*read_block)(void* self, uintptr_t lba, void* buffer, size_t byte_count);
} storage_device_class_t;

typedef struct storage_device {
    object_t super;

    // link to the rest of entries
    list_entry_t link;

    // the drive name
    // TODO: Should this be unique?
    char name[255];

    // the amount of addressable blocks
    uint64_t block_count;

    // the size of a single block
    uint64_t block_size;

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

// TODO: allow for reading multiple sectors at a time
error_t storage_read_block(void* _self, uintptr_t lba, void* buffer, size_t byte_count);

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
