#include <tlsf/tlsf.h>
#include <arch/paging.h>
#include <util/def.h>

#include "mm.h"
#include "pmm.h"
#include "vmm.h"

// the kernel tlsf instance
static tlsf_t g_kernel_tlsf;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Functions to allocate and free memory for the allocator, hopefully it is good enough
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void* kernel_tlsf_map(size_t* size, void* user) {
    uintptr_t base = 0;
    ASSERT(!IS_ERROR(pmm_allocate_pages(ALLOCATE_ANY, SIZE_TO_PAGES(*size), &base)));
    *size = ALIGN_UP(*size, PAGE_SIZE);
    return (void*)PHYSICAL_TO_DIRECT(base);
}

void  kernel_tlsf_unmap(void* mem, size_t size, void* user) {
    ASSERT(!IS_ERROR(pmm_free_pages(DIRECT_TO_PHYSICAL((uintptr_t)mem), size)));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Wrappers around tlsf
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void mm_init() {
    g_kernel_tlsf = tlsf_create(kernel_tlsf_map, kernel_tlsf_unmap, NULL);
    ASSERT(g_kernel_tlsf != NULL);
}

void* mm_allocate(size_t size) {
    return tlsf_calloc(g_kernel_tlsf, size);
}

void* mm_reallocate(void *ptr, size_t size) {
    return tlsf_realloc(g_kernel_tlsf, ptr, size);
}

void mm_free(void *ptr) {
    return tlsf_free(g_kernel_tlsf, ptr);
}
