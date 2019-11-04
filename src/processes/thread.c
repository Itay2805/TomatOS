#include <libc/string.h>
#include <memory/pmm.h>
#include "thread.h"

void thread_init(thread_t* thread, process_t* parent, bool kernel) {
    ASSERT(thread != NULL);
    ASSERT(parent != NULL);

    memset(thread, 0, sizeof(thread_t));

    thread->object.handle = generate_object_handle();
    thread->object.type = OBJECT_THREAD;

    // set default rflags
    thread->state.cpu.rflags.IF = 1;
    thread->state.cpu.rflags.ID = 1;

    // TODO: set a proper type
    pmm_allocate_pages(ALLOCATE_ANY, MEM_OTHER, SIZE_TO_PAGES(PAGE_SIZE), &thread->state.cpu.rsp);
    thread->state.cpu.rsp += PAGE_SIZE;

    if(kernel) {
        // just add the direct map to it
        thread->state.cpu.rsp = PHYSICAL_TO_DIRECT(thread->state.cpu.rsp);
    }else {
        uintptr_t userspace_stack = 0;
        ASSERT(!IS_ERROR(vmm_allocate(&parent->vmm_handle, &userspace_stack, PAGE_SIZE, PAGE_READWRITE)));

        // map it to user space area
        thread->state.cpu.rsp = userspace_stack;
    }

    // gonna add it to the parent
    aquire_lock(&parent->lock);
    insert_tail_list(&parent->threads, &thread->object.link);
    release_lock(&parent->lock);
}
