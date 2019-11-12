#include <libc/string.h>
#include "process.h"

process_t* kernel_process;

static void* Process_ctor(void* _self, va_list ap) {
    process_t* self = super_ctor(Process(), _self, ap);

    // nothing to do here for now

    return self;
}

static void* Process_dtor(void* _self) {
    process_t* self = cast(Process(), _self);

    // can't delete the kernel process
    ASSERT(self != kernel_process);

    acquire_lock(&self->lock);
    // TODO: free the vmm

    return super_dtor(Process(), self);
}

const void* Process() {
    static const void* class = NULL;
    if(class == NULL) {
        class = new(Class(),
                "Process", Object(), sizeof(process_t),
                ctor, Process_ctor,
                dtor, Process_dtor);
    }
    return class;
}
