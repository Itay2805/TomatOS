#pragma once

#include <stddef.h>
#include <assert.h>
#include <util/spinlock.hpp>
#include <arch/msr.hpp>

#ifdef VMM_NO_DIRECT_BOUNDS_CHECK
    #define CHECK_BOUNDS(...)
#else
    #define CHECK_BOUNDS  ASSERT
#endif

namespace mem::vmm {

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Direct mapping manipulation
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    constexpr uintptr_t direct_mapping_base = 0xFFFF800000000000ul;

    inline constexpr uintptr_t physical_to_direct(uintptr_t a) {
        CHECK_BOUNDS(a <= UINT64_MAX - direct_mapping_base);
        return a + direct_mapping_base;
    }

    inline constexpr uintptr_t direct_to_physical(uintptr_t a) {
        CHECK_BOUNDS(a >= direct_mapping_base);
        return a - direct_mapping_base;
    }

    inline constexpr bool is_kernel_ptr(uintptr_t ptr) {
        return ptr >= direct_mapping_base;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Page permissions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    enum permission {
        EXECUTE = 1u << 0u,
        WRITE = 1u << 1u,
        KERNEL = 1u << 2u,
        GLOBAL = 1u << 3u,
        ZERO_PAGE = 1u << 4u,
    };

    enum page_size {
        SIZE_4KB,
        SIZE_2MB,
        SIZE_1GB,
    };

    enum cache_policy {
        UNCACHEABLE = arch::IA32_PAT_MEMTYPE_UC,
        WRITE_COMBINING = arch::IA32_PAT_MEMTYPE_WC,
        WRITE_THROUGH = arch::IA32_PAT_MEMTYPE_WT,
        WRITE_PROTECTED = arch::IA32_PAT_MEMTYPE_WP,
        WRITE_BACK = arch::IA32_PAT_MEMTYPE_WB,
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Vmm functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class context {
    private:
        uintptr_t pml4_physical;
        util::spinlock lock;

    public:

        /**
         * Will create a context ready have stuff mapped to it
         */
        context();

        /**
         * Will free the context and everything related to it
         */
        ~context();

        context& operator=(const context&) = delete;
        context(const context&) = delete;

        /**
         * Will convert the given virtual address to its physical address
         *
         * TODO: Add get permissions
         *
         * @param virtual   [IN]    The virtual address
         * @param physical  [OUT]   The output physical address
         *
         * @retval true - the page was not found
         * @retval false - the page was found
         */
        bool virtual_to_physical(void* virt, uintptr_t* phys, page_size* size = nullptr);

        /**
         * Will check if the range of addresses is virtually mapped
         *
         * @param addr      [IN] The starting address to check
         * @param size      [IN] The range to check
         *
         * @retval true - the page was not found
         * @retval false - the page was found
         */
        bool is_mapped(void* virt, size_t range);

        /**
         * Will unmap the range of physical addresses
         *
         * @remark
         * This will not free the physical pages from the pmm! if you want to free them you gotta do it yourself!
         *
         * @param addr      [IN] The virtual address to unmap
         * @param size      [IN] The size to unmap
         */
        void unmap(void* addr, size_t size);

        /**
         * Will map the given range of addresses at the virtual address
         *
         * @remark
         * Thsi will not free the physical page nor actually free it if it was allocated with the vmm_allocate
         * function
         *
         * @param phys      [IN] The physical address to map from
         * @param virt      [IN] The virtual address to start at
         * @param size      [IN] The amount of bytes to map
         * @param perms     [IN] The permissions of the range
         * @param cache     [IN] The cache policy for the range
         */
        void map(uintptr_t phys, void* virt, size_t size, permission perms, cache_policy cache = WRITE_THROUGH);

        /**
         * Allocate and map a page into *userspace*
         *
         * if the virt address is set to a value which is not 0, the address will be used as the base that will be
         * used to allocate
         *
         * the page will be allocated with a default caching policy of `WRITE_THROUGH`
         *
         * @param virt      [IN/OUT]    virtual address
         * @param size      [IN]        The size that is going to be allocated
         * @param perms     [IN]        Page permissions
         */
         void allocate(uintptr_t* virt, size_t size, permission perms);

        /**
         * Free the page and unmap it
         *
         * TODO: TLB shootdown and fun stuff
         *
         * @param virt      [IN]    The virt base to free from
         * @param size      [IN]    The size to free and unmap
         */
         void free(uintptr_t virt, size_t size);

    };

    /**
     * Initialize the vmm
     */
    void init();

    /**
     * Will enable various cpu features on the current processor
     */
    void enable_cpu_features();

    /**
     *
     */
    void switch_context(context& context);

}

#undef CHECK_BOUNDS

