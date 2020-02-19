#ifndef __ARCH_MSR_H__
#define __ARCH_MSR_H__

#include <stdint.h>
#include <assert.h>

#define MSR_CODE_IA32_FS_BASE 0xC0000100
#define MSR_CODE_IA32_GS_BASE 0xC0000101
#define MSR_CODE_IA32_KERNEL_GS_BASE 0xC0000102

#define MSR_CORE_IA32_EFER                       0xC0000080
typedef union IA32_EFER {
    ///
    /// Individual bit fields
    ///
    struct {
        uint32_t  _reserved1:11;
        ///
        /// [Bit 11] Execute Disable Bit Enable.
        ///
        uint32_t  NXE:1;
        uint32_t  _reserved2:20;
        uint32_t  _reserved3:32;
    };
    uint64_t raw;
} MSR_IA32_EFER;
static_assert(sizeof(MSR_IA32_EFER) == sizeof(uint64_t));

MSR_IA32_EFER __readmsr_efer(void);
void __writemsr_efer(MSR_IA32_EFER efer);

#define MSR_CODE_IA32_APIC_BASE 0x1B
typedef union MSR_IA32_APIC_BASE {
    struct {
        uint64_t  _reserved0 : 8;
        // Is this the bootstrap processor
        uint64_t  bsp : 1;
        uint64_t  _reserved1 : 2;
        // Enables or disables the local APIC
        uint64_t  apic_global_enable : 1;
        /*
         * Base address of the APIC registers. After a
         * power-up or reset, this is set to 0xFEE00000
         */
        uint64_t  apic_base : 24;
        uint64_t  _reserved2 : 28;
    };
    uint64_t raw;
} MSR_IA32_APIC_BASE;
static_assert(sizeof(MSR_IA32_APIC_BASE) == sizeof(uint64_t));

MSR_IA32_APIC_BASE __readmsr_apic_base();

#endif //__SRC_ARCH_MSR_H__
