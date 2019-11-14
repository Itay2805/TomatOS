#ifndef TOMATKERNEL_STORAGE_OBJECT_H
#define TOMATKERNEL_STORAGE_OBJECT_H

#include <objects/object.h>
#include <util/error.h>
#include <util/sync.h>

typedef struct storage_device_class {
    class_t _;

    size_t (*get_block_size)(void* self);
    error_t (*read_block)(void* self, uintptr_t lba, void* buffer);
} storage_device_class_t;

/**
 * This is used to lock the storage objects list
 */
extern lock_t storage_obejcts_lock;

/**
 * This is an array with all the storage objects
 */
 //  TODO: Maybe convert to a linked list?
extern void** storage_objects;

const void* StorageDeviceClass();

size_t storage_get_block_size(void* _self);
error_t storage_read_block(void* _self, uintptr_t lba, void* buffer);


#endif //TOMATKERNEL_STORAGE_OBJECT_H
