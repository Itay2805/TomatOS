#include <util/except.h>
#include <util/string.h>
#include "mm.h"
#include "pmm.h"
#include "tlsf.h"

/**
 * The memory manager lock
 */
static lock_t g_mm_lock = INIT_LOCK(TPL_HIGH_LEVEL);

/**
 * The TLSF instance for the vmm
 */
static tlsf g_tlsf = TLSF_INIT;

void* tlsf_resize(tlsf* t, size_t size) {
    ASSERT(size < (KERNEL_HEAP_END - KERNEL_HEAP_START), "out of heap space");
    return (void*)KERNEL_HEAP_START;
}

void* kalloc(size_t size) {
    acquire_lock(&g_mm_lock);
    void* ptr = tlsf_malloc(&g_tlsf, size);
    release_lock(&g_mm_lock);
    memset(ptr, 0, size);
    return ptr;
}

void* krealloc(void* ptr, size_t size) {
    acquire_lock(&g_mm_lock);
    ptr = tlsf_realloc(&g_tlsf, ptr, size);
    release_lock(&g_mm_lock);
    return ptr;
}

void kfree(void* ptr) {
    acquire_lock(&g_mm_lock);
    tlsf_free(&g_tlsf, ptr);
    release_lock(&g_mm_lock);
}

directptr_t early_alloc(size_t page_count) {
    stivale2_struct_tag_memmap_t* memmap = get_stivale2_tag(STIVALE2_STRUCT_TAG_MEMMAP_ID);
    ASSERT(memmap != NULL);

    // we are only supporting page aligned allocations
    size_t size = PAGES_TO_SIZE(page_count);

    // find an entry with enough space
    for (int i = 0; i < memmap->entries; i++) {
        stivale2_mmap_entry_t* entry = &memmap->memmap[i];

        // check if a valid type
        if (entry->type == STIVALE2_MMAP_USABLE && entry->length >= size) {
            // remove the the chunk and return the pointer
            entry->length -= size;
            entry->unused += size;
            return PHYS_TO_DIRECT(entry->base) + entry->length;
        }
    }

    // no empty entries
    return NULL;
}

directptr_t early_page_alloc() {
    return early_alloc(SIZE_TO_PAGES(PAGE_SIZE));
}
