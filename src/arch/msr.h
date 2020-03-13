#ifndef __ARCH_MSR_H__
#define __ARCH_MSR_H__

#include <stdint.h>
#include <assert.h>

#define MSR_IA32_FS_BASE                         0xC0000100
#define MSR_IA32_GS_BASE                         0xC0000101
#define MSR_IA32_KERNEL_GS_BASE                  0xC0000102

#define MSR_IA32_STAR                            0xC0000081
#define MSR_IA32_LSTAR                           0xC0000082
#define MSR_IA32_CSTAR                           0xC0000083
#define MSR_IA32_FMASK                           0xC0000084

#define MSR_IA32_EFER                            0xC0000080
typedef union _IA32_EFER {
    struct {
        ///
        /// [Bit 0] SYSCALL Enable: IA32_EFER.SCE (R/W) Enables SYSCALL/SYSRET
        /// instructions in 64-bit mode.
        ///
        uint32_t  SCE:1;
        uint32_t  _reserved1:7;
        ///
        /// [Bit 8] IA-32e Mode Enable: IA32_EFER.LME (R/W) Enables IA-32e mode
        /// operation.
        ///
        uint32_t  LME:1;
        uint32_t  _reserved2:1;
        ///
        /// [Bit 10] IA-32e Mode Active: IA32_EFER.LMA (R) Indicates IA-32e mode
        /// is active when set.
        ///
        uint32_t  LMA:1;
        ///
        /// [Bit 11] Execute Disable Bit Enable: IA32_EFER.NXE (R/W).
        ///
        uint32_t  NXE:1;
        uint32_t  _reserved3:20;
        uint32_t  _reserved4:32;
    };
    uint64_t raw;
} IA32_EFER;
static_assert(sizeof(IA32_EFER) == sizeof(uint64_t));

IA32_EFER __read_efer(void);
void __write_efer(IA32_EFER efer);

#define MSR_CODE_IA32_APIC_BASE 0x1B
typedef union _IA32_APIC_BASE {
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
} IA32_APIC_BASE;
static_assert(sizeof(IA32_APIC_BASE) == sizeof(uint64_t));

IA32_APIC_BASE __read_apic_base();

#endif //__SRC_ARCH_MSR_H__
