#include "storage_object.h"


lock_t storage_objects_lock = 0;
list_entry_t storage_objects = INIT_LIST_ENTRY(storage_objects);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Storage device class implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void* StorageDeviceClass_ctor(void* _self, va_list ap) {
    storage_device_class_t* self = super_ctor(StorageDeviceClass(), _self, ap);
    void* selector;

    va_list lap;
    va_copy(lap, ap);
    while((selector = va_arg(lap, void*)) != NULL) {
        void* method = va_arg(lap, void*);

        if(selector == storage_get_block_size) {
            self->get_block_size = method;
        }else if(selector == storage_read_block) {
            self->read_block = method;
        }
    }

    return self;
}

const void* StorageDeviceClass() {
    static const void* class = NULL;
    if(class == NULL) {
        class = new(Class(),
                    "StorageDeviceClass", Class(), sizeof(storage_device_class_t),
                    ctor, StorageDeviceClass_ctor,
                    0
                );
    }
    return class;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Generic storage device implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void* StorageDevice_ctor(void* _self, va_list ap) {
    storage_device_t* self = super_ctor(StorageDevice(), _self, ap);

    self->partitions = INIT_LIST_ENTRY(self->partitions);

    return self;
}

static void* StorageDevice_dtor(void* _self) {
    super_dtor(StorageDevice(), _self);
    ASSERT(false);
}

const void* StorageDevice() {
    static const void* class = NULL;
    if(class == NULL) {
        class = new(StorageDeviceClass(),
                "StorageDevice", Object(), sizeof(storage_device_t),
                ctor, StorageDevice_ctor,
                dtor, StorageDevice_dtor,
                0);
    }
    return class;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Storage Device functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

size_t storage_get_block_count(void* _self) {
    storage_device_class_t* class = cast(StorageDeviceClass(), classOf(_self));
    ASSERT(class->get_block_count != NULL);
    return class->get_block_count(_self);
}

size_t storage_get_block_size(void* _self) {
    storage_device_class_t* class = cast(StorageDeviceClass(), classOf(_self));
    ASSERT(class->get_block_size != NULL);
    return class->get_block_size(_self);
}

error_t storage_read_block(void* _self, uintptr_t lba, void* buffer) {
    storage_device_class_t* class = cast(StorageDeviceClass(), classOf(_self));
    ASSERT(class->read_block != NULL);
    return class->read_block(_self, lba, buffer);
}

