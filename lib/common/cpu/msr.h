#ifndef TOMATKERNEL_MSR_H
#define TOMATKERNEL_MSR_H

/// Extended Feature Enable Register

#include <stdint.h>

// MSRs
#define IA32_APIC_BASE      0x1B
#define IA32_PAT            0x0277
#define IA32_EFER           0xC0000080
#define IA32_STAR           0xC0000081
#define IA32_LSTAR          0xC0000082
#define IA32_CSTAR          0xC0000083
#define IA32_FMASK          0xC0000084
#define IA32_FS_BASE        0xC0000100
#define IA32_GS_BASE        0xC0000101
#define IA32_KERNEL_GS_BASE 0xC0000102

typedef union star {
    struct {
        uint32_t target_eip;
        uint16_t syscall_cs_ss;
        uint16_t sysret_cs_ss;
    } __attribute__((packed));
    uint64_t raw;
} star_t;

typedef union efer {
    struct {
        // Syscall enable
        uint64_t syscall_enable : 1;
        uint64_t _reserved1 : 7;
        uint64_t long_mode_enable : 1;
        uint64_t _reserved2 : 1;
        uint64_t long_mode_active : 1;
        uint64_t no_execute_enable : 1;
        uint64_t _reserved3 : 52;
    } __attribute__((packed));
    uint64_t raw;
} efer_t;

#define IA32_PAT_MEMTYPE_UC     (0)
#define IA32_PAT_MEMTYPE_WC     (1)
#define IA32_PAT_MEMTYPE_WT     (4)
#define IA32_PAT_MEMTYPE_WP     (5)
#define IA32_PAT_MEMTYPE_WB     (6)
#define IA32_PAT_MEMTYPE_UCM    (7)

typedef union pat {
    struct {
        uint64_t pa0 : 3;
        uint64_t _reserved0 : 5;
        uint64_t pa1 : 3;
        uint64_t _reserved1 : 5;
        uint64_t pa2 : 3;
        uint64_t _reserved2 : 5;
        uint64_t pa3 : 3;
        uint64_t _reserved3 : 5;
        uint64_t pa4 : 3;
        uint64_t _reserved4 : 5;
        uint64_t pa5 : 3;
        uint64_t _reserved5 : 5;
        uint64_t pa6 : 3;
        uint64_t _reserved6 : 5;
        uint64_t pa7 : 3;
        uint64_t _reserved7 : 5;
    } __attribute__((packed));
    uint64_t raw;
} pat_t;

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
