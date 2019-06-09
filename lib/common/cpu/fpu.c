#include "fpu.h"
#include "cpuid.h"
#include "cr.h"

#include <common.h>

error_t fpu_init() {
    error_t err = NO_ERROR;
    cpu_features_t features;
    get_cpu_features(&features);

    log_info("Initializing FPU features");
    CHECK_ERROR_TRACE(features.fxsr, ERROR_NOT_SUPPORTED, "Missing fxsave/fxrstor instructions support");

    if(features.sse) {
        log_debug("\tSSE supported, enabling");
        if(features.sse2) log_debug("\t\tIncludes SSE2");
        if(features.sse3) log_debug("\t\tIncludes SSE3");
        if(features.ssse3) log_debug("\t\tIncludes SSSE3");
        if(features.sse41) log_debug("\t\tIncludes SSE4.1");
        if(features.sse42) log_debug("\t\tIncludes SSE4.2");
        uint64_t cr0 = get_cr0();
        cr0 &= ~CR0_EMULATION;
        cr0 |= CR0_MONITOR_CO_PROCESSOR;
        set_cr0(cr0);
        uint64_t cr4 = get_cr4();
        cr4 |= CR4_OS_SUPPORT_FXSAVE_FXSTORE;
        cr4 |= CR4_OS_SUPPORT_UNMASKED_SIMD_FP_EXCEPTIONS;
        set_cr4(cr4);
    }

//    if(features.avx) {
//        log_debug("\tAVX supported, TODO");
//    }

cleanup:
    return err;
}


error_t _fxsave(uint8_t* buf) {
    error_t err = NO_ERROR;

    CHECK(ALIGN_DOWN(buf, 16) == (uint64_t) buf);
    asm volatile("fxsave (%0)" :: "r"(buf));

cleanup:
    return err;
}

error_t _fxrstor(uint8_t* buf) {
    error_t err = NO_ERROR;

    CHECK(ALIGN_DOWN(buf, 16) == (uint64_t) buf);
    asm volatile("fxrstor (%0)" :: "r"(buf));

cleanup:
    return err;
}