#include <arch/paging.h>
#include <string.h>
#include "pmm.hpp"
#include "vmm.hpp"
#include "mm.hpp"

namespace mem {

    static allocator_policy policy;
    frg::slab_allocator<allocator_policy, util::spinlock> allocator(policy);

    uintptr_t allocator_policy::map(size_t size) const {
        return (uintptr_t)allocate_pages(arch::paging::size_to_pages(size));
    }

    void allocator_policy::unmap(uintptr_t addr, size_t size) const {
        free_pages((void*)addr, arch::paging::size_to_pages(size));
    }

    void* allocate_pages(size_t page_count) {
        uintptr_t ptr = vmm::physical_to_direct(pmm::allocate_pages(pmm::ALLOCATE_ANY, page_count));
        memset((void*)ptr, 0, arch::paging::pages_to_size(page_count));
        return (void*)ptr;
    }

    void free_pages(void* ptr, size_t page_count) {
        pmm::free_pages(vmm::direct_to_physical((uintptr_t)ptr), page_count);
    }

    typedef struct pool_alloc_head {
        uint64_t magic;
        size_t size;
        char data[0];
    } pool_alloc_head_t;

}
