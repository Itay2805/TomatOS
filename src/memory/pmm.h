#ifndef TOMATKERNEL_PMM_H
#define TOMATKERNEL_PMM_H

/*
 * Right now we are going to use a stack based approach, it is fast and doesn't change over time
 *
 * the only problem with it is the fact that it is not possible to easily allocate multiple linear
 * pages, which might become a problem once we get to doing pci/dma. Once we reach that point we
 * will either switch to a buddy allocator or just to a bitmap allocator.
 */

#include <boot/multiboot.h>

#include <error.h>
#include <stddef.h>

// TODO: Add locks to all of these

/**
 * Called at the start, will initialize stuff assuming that we still have the first gb identity mapped
 */
error_t pmm_early_init(multiboot_info_t* info);

/**
 * Will finalize everything to work assumning that we now have proper mappings and that the physical
 * memory is mapped to 0xFFFF800000000000
 */
error_t pmm_init();

/**
 * Will allocate a new physical address
 *
 * @param addr  [OUT] The new physical address
 *
 * @return NO_ERROR: Success
 * @return ERROR_OUT_OF_MEMORY: We ran out of memory!
 */
error_t pmm_allocate(uint64_t* addr);

/**
 * Will free the given *physical* address
 *
 * @param addr  [IN] The address of the page to free
 *
 * @return NO_ERROR: Success
 */
error_t pmm_free(uint64_t addr);

#endif //TOMATKERNEL_PMM_H
