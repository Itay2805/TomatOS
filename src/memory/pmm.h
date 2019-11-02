#ifndef TOMATKERNEL_MEM_H
#define TOMATKERNEL_MEM_H

#include <util/error.h>

#include <stddef.h>
#include <tboot.h>


typedef enum allocate_type {
    ALLOCATE_ANY,
    ALLOCATE_BELOW,
    ALLOCATE_ADDRESS,
} allocate_type_t;

typedef enum memory_type {
    // free memory that can be used to allocate
    MEM_FREE,

    // other memory types
    MEM_OTHER,

    // memory that the kernel allocated for itself
    MEM_KERNEL_DATA,

    // memory used by the pmm
    MEM_PMM,

    // memory used by the vmm
    MEM_VMM,

    // memory that a driver for dma
    MEM_DMA,
} memory_type_t;

/**
 * Initialize the pmm
 */
void pmm_init(tboot_info_t* info);

/**
 * Tell the pmm we are over the VMM initialization
 *
 * This is so it knows to stop using identity mapped stuff
 * and move to using direct mapped stuff
 */
void pmm_post_vmm();

/**
 * Attempt to allocate the contiguous physical pages
 *
 * If the type is ALLOCATE_ANY, it will just
 * try to allocate any given page
 *
 * If the type is ALLOCATE_BELOW it will try
 * to allocate any address which is below the
 * value in base
 *
 * If the type is ALLOCATE_ADDRESS it will attempt
 * to convert the page range into another type
 *
 * @param type          [IN]    What should the allocation do
 * @param mem_type      [IN]    The type of memory we are allocating
 * @param page_count    [IN]    The amount of pages to allocate
 * @param base          [OUT]   The allocate page (physical memory)
 */
void pmm_allocate_pages(allocate_type_t type, memory_type_t mem_type, size_t page_count, uintptr_t* base);

/**
 * Will free the pages so they can be allocated again
 *
 * @param base          [IN] Base address
 * @param page_count    [IN] The page count
 */
void pmm_free_pages(uintptr_t base, size_t page_count);

#endif //TOMATKERNEL_MEM_H
