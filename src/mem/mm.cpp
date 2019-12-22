#include <arch/paging.h>
#include <string.h>
#include "pmm.hpp"
#include "vmm.hpp"
#include "mm.hpp"

namespace mem {


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

    void* allocate_pool(size_t size) {
        size = arch::paging::size_to_pages(size + offsetof(pool_alloc_head_t, data)) * arch::paging::page_size;

        auto* ptr = (pool_alloc_head_t*)allocate_pages(arch::paging::size_to_pages(size));
        ptr->magic = 0xCAFEBABE;
        ptr->size = size;
        memset(ptr->data, 0, ptr->size - offsetof(pool_alloc_head_t, data));

        return ptr->data;
    }

    void* realloc_pool(void* ptr, size_t size) {
        if(ptr == nullptr) {
            return allocate_pool(size);
        }else {
            auto* head = (pool_alloc_head_t *) ((uintptr_t)ptr - offsetof(pool_alloc_head_t, data));
            ASSERT(head->magic == 0xCAFEBABE);

            // if already big enough to hold everything then just return the same pointer
            if(offsetof(pool_alloc_head_t, data) + size <= head->size) {
                return ptr;
            }

            // allocate and copy it
            void* newptr = allocate_pool(size);
            memcpy(newptr, ptr, head->size > size ? size : head->size);

            // free the old one
            free_pool(ptr);

            // return it
            return newptr;
        }
    }

    void free_pool(void* ptr) {
        auto* head = (pool_alloc_head_t *) ((uintptr_t)ptr - offsetof(pool_alloc_head_t, data));
        ASSERT(head->magic == 0xCAFEBABE);
        head->magic = 0xDEADBEEF;
        free_pages(head, arch::paging::size_to_pages(head->size));
    }

}
