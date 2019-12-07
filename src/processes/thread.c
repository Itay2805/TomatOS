#include <stb/stb_ds.h>
#include <memory/pmm.h>
#include <memory/mm.h>

#include <string.h>

#include "process.h"
#include "thread.h"

thread_t* new_thread(struct process* parent, void* entry) {
    thread_t* self = mm_allocate_pool(sizeof(thread_t));

    self->parent = parent;
    self->state = THREAD_STATE_WAITING;

    self->cpu_state.rflags.IF = 1;
    self->cpu_state.rflags.ID = 1;
    self->cpu_state.rip = (uint64_t) entry;

    self->kernel_stack = (uintptr_t)mm_allocate_stack(1);
    self->kernel_stack_size = PAGE_SIZE;

    if(parent == kernel_process) {
        self->stack = NULL;
        self->stack_size = 0;
        self->cpu_state.rsp = self->kernel_stack;
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
