#ifndef TOMATKERNEL_MSR_H
#define TOMATKERNEL_MSR_H

/// Extended Feature Enable Register

#include <stdint.h>

#define EFER_SYSCALL (1 << 0u)
#define EFER_LONG_MODE_ENABLE (1 << 8u)
#define EFER_LONG_MODE_ACTIVE (1 << 10u)
#define EFER_NO_EXECUTE_ENABLE (1 << 11u)
#define EFER_SECURE_VIRTUAL_MACHINE_ENABLE (1 << 12u)
#define EFER_LONG_MODE_SEGMENT_LIMIT_ENABLE (1 << 13u)
#define EFER_FAST_FXSAVE_FXSTOR (1 << 14u)
#define EFER_TRANSLATION_CACHE_EXTENSION (1 << 15u)

#define MSR_EFER 0xc0000080

// MSRs
#define IA32_EFER       0xC0000080
#define IA32_STAR       0xC0000081
#define IA32_LSTAR      0xC0000082
#define IA32_CSTAR      0xC0000083
#define IA32_SFMASK     0xC0000084
#define IA32_APIC_BASE  0x1B

static inline uint64_t _rdmsr(uint32_t msr) {
    uint32_t low, high;
    asm volatile("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));
    return ((uint64_t)high << 32u) | (uint64_t)low;
}

static inline void _wrmsr(uint32_t msr, uint64_t value) {
    asm volatile("wrmsr" : : "c"(msr), "a"(value & 0xFFFFFFFF), "d"(value >> 32));
}

static inline uint64_t _rdtsc(void) {
    uint32_t low, high;
    asm volatile("rdtsc" : "=a"(low), "=d"(high));
    return ((uint64_t)high << 32) | low;
}

#endif //TOMATKERNEL_MSR_H
