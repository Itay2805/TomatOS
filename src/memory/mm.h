#ifndef TOMATKERNEL_MM_H
#define TOMATKERNEL_MM_H

#include <stddef.h>
#include <util/error.h>

/**
 * Allocate pages
 *
 * @remark
 * This returns a direct mapped ptr, so it means you can just use it normally
 *
 * @param size  [IN] The amount of pages to allocate
 */
uintptr_t mm_allocate_pages(size_t page_count);

/**
 * Free the allocated pages
 *
 * @param ptr           [IN] The base to free from
 * @param page_count    [IN] The pages to free
 */
void mm_free_pages(uintptr_t ptr, size_t page_count);

///**
// * Allocate memory from a memory pool
// *
// * The idea is to allow for more efficient smaller allocations
// *
// * @param size  [IN] The size to allocate
// */
//void* mm_pool_allocate(size_t size);
//
///**
// * Free memory allocated from the memory pool
// *
// * @param ptr   [IN] The pointer to free
// */
//void mm_pool_free(void* ptr);

#endif //TOMATKERNEL_MM_H
