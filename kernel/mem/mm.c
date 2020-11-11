#include <util/except.h>
#include <util/string.h>
#include "mm.h"
#include "pmm.h"
#include "tlsf.h"

void dump_kernel_mappings() {
    TRACE("Kernel mappings:");
    TRACE("\tDirect map   | ", (void*)DIRECT_BASE, "-", (void*)DIRECT_END);
    TRACE("\tPMM refcount | ", (void*)PAGE_REFCOUNT_START, "-", (void*)PAGE_REFCOUNT_END);
    TRACE("\tkernel heap  | ", (void*)KERNEL_HEAP_START, "-", (void*)KERNEL_HEAP_END);
}

// TODO: maybe have a pool per cpu for better performance
static ticket_lock_t g_mm_lock = INIT_LOCK();
static tlsf g_tlsf = TLSF_INIT;

void* tlsf_resize(tlsf* t, size_t size) {
    ASSERT(size < (KERNEL_HEAP_END - KERNEL_HEAP_START), "out of heap space");
    return (void*)KERNEL_HEAP_START;
}

void* kalloc(size_t size) {
    ticket_lock(&g_mm_lock);
    void* ptr = tlsf_malloc(&g_tlsf, size);
    ticket_unlock(&g_mm_lock);
    memset(ptr, 0, size);
    return ptr;
}

void* krealloc(void* ptr, size_t size) {
    ticket_lock(&g_mm_lock);
    ptr = tlsf_realloc(&g_tlsf, ptr, size);
    ticket_unlock(&g_mm_lock);
    return ptr;
}

void kfree(void* ptr) {
    ticket_lock(&g_mm_lock);
    tlsf_free(&g_tlsf, ptr);
    ticket_unlock(&g_mm_lock);
}

