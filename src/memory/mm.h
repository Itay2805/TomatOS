#ifndef TOMATKERNEL_MM_H
#define TOMATKERNEL_MM_H

#include <stddef.h>

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

#endif //TOMATKERNEL_MM_H
