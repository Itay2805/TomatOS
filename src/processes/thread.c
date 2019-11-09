#include <libc/string.h>
#include <memory/pmm.h>
#include <memory/mm.h>
#include "thread.h"

void thread_init(thread_t* thread, process_t* parent) {
    ASSERT(thread != NULL);
    ASSERT(parent != NULL);

    memset(thread, 0, sizeof(thread_t));

    thread->object.handle = generate_object_handle();
    thread->object.type = OBJECT_THREAD;

    // set general thread stuff
    thread->parent = parent;
    thread->state = THREAD_WAITING;

    // set default rflags
    thread->cpu_state.rflags.IF = 1;
    thread->cpu_state.rflags.ID = 1;

    // allocate the kernel stack (used on syscall)
    thread->kernel_stack = mm_allocate_pages(1);
    thread->kernel_stack_size = PAGE_SIZE;

    // set the process stack
    if(parent == &kernel_process) {
        thread->stack = NULL;
        thread->stack_size = 0;
        thread->cpu_state.rsp = thread->kernel_stack + thread->kernel_stack_size;
    }else {
        thread->stack_size = PAGE_SIZE;
        ASSERT(!IS_ERROR(vmm_allocate(&parent->vmm_handle, &thread->stack, thread->stack_size, PAGE_READWRITE)));
        thread->cpu_state.rsp = thread->stack + thread->stack_size;
    }

    // gonna add it to the parent
    acquire_lock(&parent->lock);
    insert_tail_list(&parent->threads, &thread->object.link);
    release_lock(&parent->lock);
}
