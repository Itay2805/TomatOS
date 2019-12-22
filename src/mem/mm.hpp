#pragma once

#include <stddef.h>

namespace mem {

    /**
     * Allocate pages
     *
     * @remark
     * This returns a direct mapped ptr, so it means you can just use it normally
     *
     * @param size  [IN] The amount of pages to allocate
     */
    void* allocate_pages(size_t page_count);

    /**
     * Free the allocated pages
     *
     * @param ptr           [IN] The base to free from
     * @param page_count    [IN] The pages to free
     */
    void free_pages(void* ptr, size_t page_count);

    /**
     * Allocate memory from a memory pool
     *
     * The idea is to allow for more efficient smaller allocations
     *
     * @param size  [IN] The size to allocate
     */
    void* allocate_pool(size_t size);

    /**
     * Reallocate an element in the pool
     *
     * @param ptr   [IN] The old pointer
     * @param size  [IN] The new size to allocate
     */
    void* realloc_pool(void* ptr, size_t size);

    /**
     * Free memory allocated from the memory pool
     *
     * @param ptr   [IN] The pointer to free
     */
    void free_pool(void* ptr);

}
