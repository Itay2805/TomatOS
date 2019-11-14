#include <stb/stb_ds.h>
#include <memory/pmm.h>
#include <memory/mm.h>

#include <string.h>

#include "process.h"
#include "thread.h"

static void* Thread_ctor(void* _self, va_list ap) {
    thread_t* self = super_ctor(Thread(), _self, ap);

    process_t* parent = va_arg(ap, process_t*);

    self->parent = parent;
    self->state = THREAD_WAITING;

    self->cpu_state.rflags.IF = 1;
    self->cpu_state.rflags.ID = 1;
    self->cpu_state.rip = va_arg(ap, uintptr_t);

    self->kernel_stack = (uintptr_t)mm_allocate_pages(1);
    self->kernel_stack_size = PAGE_SIZE;

    if(parent == kernel_process) {
        self->stack = NULL;
        self->stack_size = 0;
        self->cpu_state.rsp = self->kernel_stack + self->kernel_stack_size;
    }else {
        self->stack_size = PAGE_SIZE;
        ASSERT(!IS_ERROR(vmm_allocate(&parent->vmm_handle, &self->stack, self->stack_size, PAGE_READWRITE)));
        self->cpu_state.rsp = self->stack + self->stack_size;
    }

    acquire_lock(&parent->lock);
    arrpush(parent->threads, self);
    release_lock(&parent->lock);

    return self;
}

static void* Thread_dtor(void* _self, va_list ap) {
    // TODO: this
    ASSERT(0);
}

const void* Thread() {
    static const void* class = NULL;
    if(class == NULL) {
        class = new(Class(),
                "Thread", Object(), sizeof(thread_t),
                ctor, Thread_ctor,
                dtor, Thread_dtor);
    }
    return class;
}

