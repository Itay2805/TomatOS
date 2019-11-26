#include <libc/string.h>
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
