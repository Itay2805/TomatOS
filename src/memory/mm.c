#include <libc/string.h>
#include <processes/process.h>
#include "mm.h"

#include "pmm.h"
#include "vmm.h"

// TODO: return pointer instead of uintptr
void* mm_allocate_pages(size_t page_count) {
    uintptr_t ptr;
    pmm_allocate_pages(ALLOCATE_ANY, MEM_KERNEL_DATA, page_count, &ptr);
    memset(PHYSICAL_TO_DIRECT((void*)ptr), 0, PAGES_TO_SIZE(page_count));
    return PHYSICAL_TO_DIRECT((void*)ptr);
}

void mm_free_pages(void* ptr, size_t page_count) {
    pmm_free_pages(DIRECT_TO_PHYSICAL((uintptr_t)ptr), page_count);
}

uintptr_t mm_allocate_stack(size_t page_count) {
    uintptr_t stack = (uintptr_t)mm_allocate_pages(page_count + 2);

    // unmap the first and last page to act as a guard
    vmm_unmap(&kernel_process->vmm_handle, stack, PAGE_SIZE);
    vmm_unmap(&kernel_process->vmm_handle, stack + PAGE_SIZE + PAGES_TO_SIZE(page_count), PAGE_SIZE);

    // return the address before the last page
    return stack + PAGE_SIZE + PAGES_TO_SIZE(page_count);
}

void mm_free_stack(uintptr_t stack, size_t page_count) {
    uintptr_t actual_stack_base = stack - PAGE_SIZE - page_count;

    // map the first and last page
    vmm_map(&kernel_process->vmm_handle, DIRECT_TO_PHYSICAL(actual_stack_base), actual_stack_base, PAGE_SIZE, PAGE_SUPERVISOR_READWRITE, DEFAULT_CACHE);
    vmm_map(&kernel_process->vmm_handle, DIRECT_TO_PHYSICAL(stack + PAGE_SIZE + PAGES_TO_SIZE(page_count)), stack + PAGE_SIZE + PAGES_TO_SIZE(page_count), PAGE_SIZE, PAGE_SUPERVISOR_READWRITE, DEFAULT_CACHE);

    // free it all
    mm_free_pages((void*)actual_stack_base, page_count + 2);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Pool implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Right now it just calls the allocate pages, soon enough I will implement something proper
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


typedef struct pool_alloc_head {
    uint64_t magic;
    size_t size;
    char data[0];
} pool_alloc_head_t;

void* mm_allocate_pool(size_t size) {
    size = SIZE_TO_PAGES(size + offsetof(pool_alloc_head_t, data)) * PAGE_SIZE;

    pool_alloc_head_t* ptr = (pool_alloc_head_t*)mm_allocate_pages(SIZE_TO_PAGES(size));
    ptr->magic = 0xCAFEBABE;
    ptr->size = size;
    memset(ptr->data, 0, ptr->size - offsetof(pool_alloc_head_t, data));

    return ptr->data;
}

void* mm_pool_realloc(void* ptr, size_t size) {
    if(ptr == NULL) {
        return mm_allocate_pool(size);
    }else {
        pool_alloc_head_t* head = (pool_alloc_head_t *) ((uintptr_t)ptr - offsetof(pool_alloc_head_t, data));
        ASSERT(head->magic == 0xCAFEBABE);

        // allocate and copy it
        void* newptr = mm_allocate_pool(size);
        memcpy(newptr, ptr, head->size > size ? size : head->size);

        // free the old one
        mm_free_pool(ptr);

        // return it
        return newptr;
    }
}

void mm_free_pool(void* ptr) {
    pool_alloc_head_t* head = (pool_alloc_head_t *) ((uintptr_t)ptr - offsetof(pool_alloc_head_t, data));
    ASSERT(head->magic == 0xCAFEBABE);
    head->magic = 0xDEADBEEF;
    mm_free_pages(head, SIZE_TO_PAGES(head->size));
}
