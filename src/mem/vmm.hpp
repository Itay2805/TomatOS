#pragma once

#include <stddef.h>
#include <assert.h>

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

    enum permission : char {
        EXECUTE = 1 << 0,
        WRITE = 2 << 0,
        KERNEL = 3 << 0,
        ZERO_PAGE = 4 << 0
    };

    inline constexpr permission operator|(permission a, permission b) {
        return a = static_cast<permission>(a | b);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Vmm functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    struct vmm_handle {
        uintptr_t pml4_physical;
    };



}

#undef CHECK_BOUNDS

