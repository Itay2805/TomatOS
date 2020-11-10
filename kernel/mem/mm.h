#ifndef TOMATOS_MM_H
#define TOMATOS_MM_H

#include <util/defs.h>

typedef void* directptr_t;
typedef uint64_t physptr_t;

#define PAGE_SIZE               (SIZE_4KB)
#define MAX_PHYS_MEMORY_SIZE    (SIZE_64TB)

#define KERNEL_BASE             (0xffffffff80000000ull)

#define DIRECT_BASE             (0xFFFF800000000000ull)
#define DIRECT_END              (0xFFFF800000000000ull + MAX_PHYS_MEMORY_SIZE)

#define PAGE_REFCOUNT_START     (DIRECT_END + SIZE_1GB)
#define PAGE_REFCOUNT_SIZE      (ALIGN_UP((MAX_PHYS_MEMORY_SIZE / PAGE_SIZE) * sizeof(uint16_t), PAGE_SIZE))
#define PAGE_REFCOUNT_END            (PAGE_REFCOUNT_START + PAGE_REFCOUNT_SIZE)

#define KERNEL_HEAP_START       (PAGE_REFCOUNT_END + SIZE_1GB)
#define KERNEL_HEAP_END         (KERNEL_HEAP_START + SIZE_32TB)

_Static_assert(KERNEL_BASE > KERNEL_HEAP_END, "kernel overlaps with virtual memory regions");

#define DIRECT_TO_PHYS(dir) ((physptr_t)((uint8_t*)(dir) - DIRECT_BASE))
#define PHYS_TO_DIRECT(dir) ((directptr_t)((uint8_t*)(dir) + DIRECT_BASE))

/**
 * Prints the memory mappings for debug reasons
 */
void dump_kernel_mappings();

void* kalloc(size_t size);
void* krealloc(void* ptr, size_t size);
void kfree(void* ptr);

#endif //TOMATOS_MM_H
