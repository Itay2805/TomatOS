#ifndef TOMATOS_MM_H
#define TOMATOS_MM_H

#include <util/defs.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// General definitions of the memory map
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Page size
 */
#define PAGE_SIZE               (SIZE_4KB)
#define PAGES_TO_SIZE(x)        ((x) * PAGE_SIZE)
#define SIZE_TO_PAGES(x)        (ALIGN_UP(x, PAGE_SIZE) / PAGE_SIZE)

/**
 * The kernel base
 */
#define KERNEL_BASE             (0xffffffff80000000ull)

/**
 * The direct memory base and end (64TB)
 */
#define MAX_PHYS_MEMORY_SIZE    (SIZE_64TB)
#define DIRECT_BASE             (0xFFFF800000000000ull)
#define DIRECT_END              (0xFFFF800000000000ull + MAX_PHYS_MEMORY_SIZE)

/**
 * The kernel heap ranges (has 32TB)
 * TODO: move this far away from the direct map maybe?
 */
#define KERNEL_HEAP_START       (DIRECT_END + SIZE_1GB)
#define KERNEL_HEAP_END         (KERNEL_HEAP_START + SIZE_32TB)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Direct Map
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Physical to direct and the opposite
 */

typedef void* directptr_t;
typedef uint64_t physptr_t;

#define DIRECT_TO_PHYS(dir) ((physptr_t)((uint8_t*)(dir) - DIRECT_BASE))
#define PHYS_TO_DIRECT(dir) ((directptr_t)((uint8_t*)(dir) + DIRECT_BASE))

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Heap allocators
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * TODO: This
 */
void* kalloc(size_t size);

/**
 * TODO: This
 */
void* krealloc(void* ptr, size_t size);

/**
 * TODO: This
 */
void kfree(void* ptr);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Early allocators
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * TODO: This
 */
directptr_t early_alloc(size_t page_count);

/**
 * TODO: This
 */
directptr_t early_page_alloc();

#endif //TOMATOS_MM_H
