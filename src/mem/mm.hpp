#pragma once

#include <stddef.h>
#include <frg/slab.hpp>
#include <util/spinlock.hpp>

namespace mem {

    /**
     * This is the policy of the allocator
     * will simply allow the allocator to map
     * and unmap memory as it needs
     */
    struct allocator_policy {
        [[nodiscard]]
        uintptr_t map(size_t size) const;
        void unmap(uintptr_t addr, size_t size) const;
    };

    /**
     * This is the allocator used for memory allocations
     */
    extern frg::slab_allocator<allocator_policy, util::spinlock> allocator;

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

}
