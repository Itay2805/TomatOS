#include <libc/string.h>
#include "mm.h"

#include "pmm.h"
#include "vmm.h"

uintptr_t mm_allocate_pages(size_t page_count) {
    uintptr_t ptr;
    pmm_allocate_pages(ALLOCATE_ANY, MEM_KERNEL_DATA, page_count, &ptr);
    return PHYSICAL_TO_DIRECT(ptr);
}

void mm_free_pages(uintptr_t ptr, size_t page_count) {
    pmm_free_pages(DIRECT_TO_PHYSICAL(ptr), page_count);
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

void* mm_pool_allocate(size_t size) {
    size = SIZE_TO_PAGES(size + offsetof(pool_alloc_head_t, data)) * PAGE_SIZE;

    pool_alloc_head_t* ptr = (pool_alloc_head_t*)mm_allocate_pages(SIZE_TO_PAGES(size));
    ptr->magic = 0xCAFEBABE;
    ptr->size = size;
    memset(ptr->data, 0, ptr->size - offsetof(pool_alloc_head_t, data));

    return ptr->data;
}

void* mm_pool_realloc(void* ptr, size_t size) {
    pool_alloc_head_t* head = (pool_alloc_head_t *) ((uintptr_t)ptr - offsetof(pool_alloc_head_t, data));
    ASSERT(head->magic == 0xCAFEBABE);

    // allocate and copy it
    void* newptr = mm_pool_allocate(size);
    memcpy(newptr, ptr, head->size > size ? size : head->size);

    // free the old one
    mm_pool_free(ptr);

    // return it
    return newptr;
}

void mm_pool_free(void* ptr) {
    pool_alloc_head_t* head = (pool_alloc_head_t *) ((uintptr_t)ptr - offsetof(pool_alloc_head_t, data));
    ASSERT(head->magic == 0xCAFEBABE);
    head->magic = 0xDEADBEEF;
    mm_free_pages((uintptr_t) head, head->size);
}

