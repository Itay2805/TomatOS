#include "storage_object.h"

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

size_t storage_get_block_size(void* _self) {
    storage_device_class_t* class = cast(StorageDeviceClass(), classOf(_self));
    return class->get_block_size(_self);
}

error_t storage_read_block(void* _self, uintptr_t lba, void* buffer) {
    storage_device_class_t* class = cast(StorageDeviceClass(), classOf(_self));
    return class->read_block(_self, lba, buffer);
}