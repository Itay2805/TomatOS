#include <string.h>
#include "partition.h"

static void* Partition_ctor(void* _self, va_list ap) {
    partition_t* self = super_ctor(Partition(), _self, ap);

    self->lba_start = va_arg(ap, uint64_t);
    self->lba_end = va_arg(ap, uint64_t);

    strncpy(self->name, va_arg(ap, char*), sizeof(self->name) - 1);

    return self;
}

static void* Partition_dtor(void* _self) {
    partition_t* self = super_dtor(Partition(), _self);

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