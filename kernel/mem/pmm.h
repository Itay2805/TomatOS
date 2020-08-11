#ifndef __TOMATOS_KERNEL_ARCH_PMM_H__
#define __TOMATOS_KERNEL_ARCH_PMM_H__

#include <util/except.h>
#include <util/defs.h>
#include "vmm.h"

/**
 * Add a physical memory range from the pmm
 *
 * @param base  [IN] The base of the memory to add
 * @param size  [IN] The size of the range
 */
void pmm_add_range(directptr_t base, size_t size);

/**
 * Allocate physical memory from low memory (<4GB) should not be used unless specifically
 * needed for a certain device that does not support 64bit addresses
 *
 * @remark
 * This function will ASSERT if it failed to allocate physical memory
 *
 * @param size  [IN]    The amount of memory to allocate
 */
directptr_t pmalloc_low(size_t size);

/**
 * Allocate physical memory
 *
 * will first try to allocate from higher memory (>4GB) and if failed
 * will try to allocate from low memory
 *
 * @remark
 * This function will ASSERT if it failed to allocate physical memory
 *
 * @remark
 * The allocated memory is not zeroed by default
 *
 * @param size  [IN]    The amount of memory to allocate
 */
directptr_t pmalloc(size_t size);

/**
 * Like pmalloc, but zeros the memory
 *
 * @param size  [IN]    The amount of memory to allocate
 */
directptr_t pmallocz(size_t size);

/**
 * Like pmalloc_low, but zeros the memory
 *
 * @param size  [IN]    The amount of memory to allocate
 */
directptr_t pmallocz_low(size_t size);

/**
 * Free physical memory
 *
 * @param ptr   [IN]    The pointer to free
 * @param size  [IN]    The size to free
 */
void pmfree(directptr_t ptr, size_t size);

/**
 * Initialize the ref-counted page management
 */
void pmalloc_page_init();

/**
 * Will allocate a single page.
 */
directptr_t pmalloc_page();

/**
 * Will increment the ref count for the given page
 *
 * @param page  [IN] The page
 */
void pmalloc_ref_page(directptr_t page);

/**
 * Will free (decrement the ref count) of the given page
 *
 * @param page  [IN] The page
 */
void pmfree_page(directptr_t page);

#endif //__TOMATOS_KERNEL_ARCH_PMM_H__
