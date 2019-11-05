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
