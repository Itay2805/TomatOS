#ifndef TOMATKERNEL_CR_H
#define TOMATKERNEL_CR_H

#include <stdint.h>

#define CR0_PROTECTED_MODE_ENABLE (1u << 0u)
#define CR0_MONITOR_CO_PROCESSOR (1u << 1u)
#define CR0_EMULATION (1u << 2u)
#define CR0_TASK_SWITCHED (1u << 3u)
#define CR0_EXTENSION_TYPE (1u << 4u)
#define CR0_NUMERIC_ERROR (1u << 5u)
#define CR0_WRITE_PROTECT (1u << 16u)
#define CR0_ALIGNMENT_CHECK (1u << 18u)
#define CR0_NOT_WRITE_THROUGH (1u << 29u)
#define CR0_CACHE_DISABLE (1u << 30u)
#define CR0_PAGING (1u << 31u)

#define CR3_PCID_MASK 0xfffu
#define CR3_PML4_MASK 0xfffffffffffff000u

#define CR4_V8086_MODE_EXTENSIONS (1u << 0u)
#define CR4_PROTECTED_MODE_VIRTUAL_INTERRUPTS (1u << 1u)
#define CR4_TIME_STAMP_RESTRICT_RING_0 (1u << 2u)
#define CR4_DEBUGGING_EXTENSIONS (1u << 3u)
#define CR4_PAGE_SIZE_EXTENSIONS (1u << 4u)
#define CR4_PHYSICAL_ADDRESS_EXTENSION (1u << 5u)
#define CR4_MACHINE_CHECK_EXCEPTION (1u << 6u)
#define CR4_PAGE_GLOBAL_ENABLED (1u << 7u)
#define CR4_PERFORMANCE_MONITORING_COUNTER_ENABLE (1u << 8u)
#define CR4_OS_SUPPORT_FXSAVE_FXSTORE (1u << 9u)
#define CR4_OS_SUPPORT_UNMASKED_SIMD_FP_EXCEPTIONS (1u << 10u)
#define CR4_USER_MODE_INSTRUCTION_PREVENTION (1u << 11u)
#define CR4_VIRTUAL_MACHINE_EXTENSIONS_ENABLED (1u << 13u)
#define CR4_SAFER_MODE_EXTENSIONS_ENABLED (1u << 14u)
#define CR4_PCID_ENABLED (1u << 17u)
#define CR4_XSAVE_PROCESSOR_EXTENDED_STATES_ENABLED (1u << 18u)
#define CR4_SUPERVISOR_MODE_EXECUTION_PREVENTION_ENABLE (1u << 20u)
#define CR4_SUPERVISOR_MODE_ACCESS_PROTECTION_ENABLE (1u << 21u)

static inline uint64_t get_cr0() {
    unsigned long val;
    asm volatile ( "mov %%cr0, %0" : "=r"(val) );
    return val;
}

static inline void set_cr0(uint64_t val) {
    asm volatile ( "mov %0, %%cr0" : : "r"(val) );
}

static inline uint64_t get_cr2() {
    unsigned long val;
    asm volatile ( "mov %%cr2, %0" : "=r"(val) );
    return val;
}

static inline void set_cr2(uint64_t val) {
    asm volatile ( "mov %0, %%cr2" : : "r"(val) );
}

static inline uint64_t get_cr3() {
    unsigned long val;
    asm volatile ( "mov %%cr3, %0" : "=r"(val) );
    return val;
}

static inline void set_cr3(uint64_t val) {
    asm volatile ( "mov %0, %%cr3" : : "r"(val) );
}

static inline uint64_t get_cr4() {
    unsigned long val;
    asm volatile ( "mov %%cr4, %0" : "=r"(val) );
    return val;
}

static inline void set_cr4(uint64_t val) {
    asm volatile ( "mov %0, %%cr4" : : "r"(val) );
}

#endif //TOMATKERNEL_CR_H