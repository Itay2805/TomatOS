#ifndef TOMATKERNEL_MSR_H
#define TOMATKERNEL_MSR_H

#include <common/stdint.h>

////////////////////////////////////////////////////////////////////////////
// Machine Specific Registers (MSR)
////////////////////////////////////////////////////////////////////////////
//
// This allows to easily use msrs for enabling special features
//
////////////////////////////////////////////////////////////////////////////

/// Extended Feature Enable Register
#define EFER_SYSCALL (1 << 0u)
#define EFER_LONG_MODE_ENABLE (1 << 8u)
#define EFER_LONG_MODE_ACTIVE (1 << 10u)
#define EFER_NO_EXECUTE_ENABLE (1 << 11u)
#define EFER_SECURE_VIRTUAL_MACHINE_ENABLE (1 << 12u)
#define EFER_LONG_MODE_SEGMENT_LIMIT_ENABLE (1 << 13u)
#define EFER_FAST_FXSAVE_FXSTOR (1 << 14u)
#define EFER_TRANSLATION_CACHE_EXTENSION (1 << 15u)

#define MSR_EFER 0xc0000080
#define MSR_STAR 0xc0000081
#define MSR_LSTAR 0xc0000082
#define MSR_CSTAR 0xc0000083
#define MSR_SFMASK 0xc0000084
#define MSR_FS_BASE 0xc0000100
#define MSR_GS_BASE 0xc0000101
#define MSR_KERNEL_GS_BASE 0xc0000102

static inline uint64_t rdmsr(uint32_t msr) {
    uint32_t low, high;
    asm volatile("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));
    return ((uint64_t)high << 32u) | (uint64_t)low;
}

static inline void wrmsr(uint32_t msr, uint64_t value) {
    asm volatile("wrmsr" : : "c"(msr), "a"(value & 0xFFFFFFFF), "d"(value >> 32));
}

#endif //TOMATKERNEL_MSR_H
