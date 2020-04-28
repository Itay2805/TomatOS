#include <stddef.h>
#include <util/trace.h>
#include <stdbool.h>
#include <util/except.h>
#include <mm/pmm.h>
#include <mm/vmm.h>
#include <string.h>

#include "cpuid.h"
#include "simd.h"
#include "intrin.h"
#include "paging.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementations for restore/save
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void (*save_simd_state)(void* state);
void (*restore_simd_state)(void* state);

static void xsave_save_state(void* state) {
    __xsave(state);
}

static void xsave_restore_state(void* state) {
    __xrstor(state);
}

static void fxsr_save_state(void* state) {
    __fxsave(state);
}

static void fxsr_restore_state(void* state) {
    __fxrstor(state);
}

static void null_save_state(void* state) {
    // do nothing
}

static void null_restore_state(void* state) {
    // do nothing
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Initialization
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Do we have xsave, xcr0 is gonna be set only
 * if this is true
 */
static bool m_has_xsave = false;

/**
 * Do we have fxsr?
 */
static bool m_has_fxsr = false;

/**
 * The FPU state size
 */
static uint32_t m_state_size = 0;

/*
 * The xcr0 that we are gonna set for all cpus
 */
static IA32_XCR0 m_xcr0 = { .raw = 0 };

void init_simd_for_bsp() {
    CPUID_VERSION_INFO_ECX version_info_ecx = { .raw = 0 };
    CPUID_VERSION_INFO_EDX version_info_edx = { .raw = 0 };
    cpuid(CPUID_VERSION_INFO, NULL, NULL, &version_info_ecx.raw, &version_info_edx.raw);

    TRACE("SIMD CPU State features:");

    /***************************************************
     * XSAVE Support - optional
     **************************************************/
    if (version_info_ecx.XSAVE) {
        m_has_xsave = true;

        save_simd_state = xsave_save_state;
        restore_simd_state = xsave_restore_state;

        CPUID_EXTENDED_STATE_MAIN_LEAF_EAX extended_state_main_leaf_eax = { .raw = 0 };
        cpuidex(CPUID_EXTENDED_STATE, CPUID_EXTENDED_STATE_MAIN_LEAF, &extended_state_main_leaf_eax.raw, NULL, &m_state_size, NULL);

        TRACE("\t* XSAVE (%d bytes)", m_state_size);

        /***************************************************
         * Check for other features
         **************************************************/
        if (extended_state_main_leaf_eax.x87) {
            TRACE("\t\t* x87 state");
            m_xcr0.x87 = 1;
        }

        if (extended_state_main_leaf_eax.SSE) {
            TRACE("\t\t* SSE state");
            m_xcr0.SSE = 1;
        }

        if (extended_state_main_leaf_eax.AVX) {
            TRACE("\t\t* AVX state");
            m_xcr0.AVX = 1;
        }

        if (extended_state_main_leaf_eax.AVX_512) {
            TRACE("\t\t* AVX-512 state");
            m_xcr0.AVX_512 = 1;
        }

        // finally set the xcr0 for this cpu
        IA32_CR4 cr4 = __readcr4();
        cr4.OSXSAVE = 1;
        __writecr4(cr4);

        __xsetbv(0, m_xcr0.raw);

    /***************************************************
     * FXSR Suppoer - fallback for XSAVE
     **************************************************/
    } else if (version_info_edx.FXSR) {
        m_has_fxsr = true;

        save_simd_state = fxsr_save_state;
        restore_simd_state = fxsr_restore_state;

        m_state_size = 512;
        TRACE("\t* FXSR");

        // finally enable fxsr
        IA32_CR4 cr4 = __readcr4();
        cr4.OSFXSR = 1;
        __writecr4(cr4);

    /***************************************************
     * No support
     **************************************************/
     } else {
        save_simd_state = null_save_state;
        restore_simd_state = null_restore_state;

        TRACE("!!! NO FXSR/XSAVE SUPPORT !!!");
    }

    // setup stuff for fpu/sse
    if (m_has_fxsr || m_has_xsave) {
        IA32_CR0 cr0 = __readcr0();
        cr0.EM = 0;
        cr0.MP = 1;
        __writecr0(cr0);

        IA32_CR4 cr4 = __readcr4();
        cr4.OSXMMEXCPT = 1;
        __writecr4(cr4);
    }
}

err_t init_simd_for_cpu() {
    err_t err = NO_ERROR;

    // setup stuff for fpu/sse
    if (m_has_fxsr || m_has_xsave) {
        IA32_CR0 cr0 = __readcr0();
        cr0.EM = 0;
        cr0.MP = 1;
        __writecr0(cr0);

        IA32_CR4 cr4 = __readcr4();
        cr4.OSXMMEXCPT = 1;
        __writecr4(cr4);
    }

    // check for xsave
    if (m_has_xsave) {
        CPUID_VERSION_INFO_ECX version_info_ecx = { .raw = 0 };
        cpuid(CPUID_VERSION_INFO, NULL, NULL, &version_info_ecx.raw, NULL);
        CHECK_ERROR_TRACE(version_info_ecx.XSAVE, ERROR_NOT_FOUND, "AP does not support XSAVE unlike the BSP");

        // check that the cpu support all the stuff that we enabled
        CPUID_EXTENDED_STATE_MAIN_LEAF_EAX extended_state_main_leaf_eax = { .raw = 0 };
        cpuidex(CPUID_EXTENDED_STATE, CPUID_EXTENDED_STATE_MAIN_LEAF, &extended_state_main_leaf_eax.raw, NULL, NULL, NULL);
        CHECK_ERROR_TRACE((extended_state_main_leaf_eax.raw & m_xcr0.raw) == m_xcr0.raw, ERROR_NOT_FOUND, "AP does not support all SIMD extensions as the BSP");

        // setup CR4
        IA32_CR4 cr4 = __readcr4();
        cr4.OSXSAVE = 1;
        __writecr4(cr4);

        // set xcr0
        __xsetbv(0, m_xcr0.raw);

    // check for fxsr
    } else if(m_has_fxsr) {
        CPUID_VERSION_INFO_EDX version_info_edx = { .raw = 0 };
        cpuid(CPUID_VERSION_INFO, NULL, NULL, NULL, &version_info_edx.raw);

        // setup CR4
        IA32_CR4 cr4 = __readcr4();
        cr4.OSFXSR = 1;
        __writecr4(cr4);
    }

cleanup:
    return err;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// API Implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void* create_simd_state() {
    if (m_state_size == 0) {
        return NULL;
    } else {
        // allocate it
        uintptr_t base;
        ASSERT(!IS_ERROR(pmm_allocate_pages(ALLOCATE_ANY, SIZE_TO_PAGES(m_state_size), &base)));

        // setup the correct context
        FX_SAVE_STATE* state = (void*)PHYSICAL_TO_DIRECT(base);
        memset(state, 0, m_state_size);

        state->Fcw |= 1 << 0; // Set Invalid Operation Mask
        state->Fcw |= 1 << 1; // Set Denormal Operand Mask
        state->Fcw |= 1 << 2; // Set Divide by Zero Mask
        state->Fcw |= 1 << 3; // Set Overflow Mask
        state->Fcw |= 1 << 4; // Set Underflow Mask
        state->Fcw |= 1 << 5; // Set Precision Mask
        state->Fcw |= (1 << 8) | (1 << 9); // Set Double Extended Precision

        state->Mxcsr |= 1 << 7; // Set Invalid Operation Mask
        state->Mxcsr |= 1 << 8; // Set Denormal Mask
        state->Mxcsr |= 1 << 9; // Set Divide by Zero Mask
        state->Mxcsr |= 1 << 10; // Set Overflow Mask
        state->Mxcsr |= 1 << 11; // Set Underflow Mask
        state->Mxcsr |= 1 << 12; // Set Precision Mask

        return state;
    }
}

void free_simd_state(void* state) {
    if (m_state_size != 0) {
        ASSERT(!IS_ERROR(pmm_free_pages((uintptr_t)DIRECT_TO_PHYSICAL(state), SIZE_TO_PAGES(m_state_size))));
    } else {
        ASSERT(state == NULL);
    }
}
