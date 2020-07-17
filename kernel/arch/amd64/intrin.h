#ifndef __TOMATOS_KERNEL_ARCH_AMD64_INTRIN_H__
#define __TOMATOS_KERNEL_ARCH_AMD64_INTRIN_H__

#include <stdint.h>
#include "gdt.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Port io operations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void __outb(uint16_t port, uint8_t data);
void __outw(uint16_t port, uint16_t data);
void __outl(uint16_t port, uint32_t data);

uint8_t __inb(uint16_t port);
uint16_t __inw(uint16_t port);
uint32_t __inl(uint16_t port);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Control registers operations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint64_t __readcr8(void);
void __writecr8(uint64_t Data);

typedef union {
    struct {
        uint32_t  VME:1;          ///< Virtual-8086 Mode Extensions.
        uint32_t  PVI:1;          ///< Protected-Mode Virtual Interrupts.
        uint32_t  TSD:1;          ///< Time Stamp Disable.
        uint32_t  DE:1;           ///< Debugging Extensions.
        uint32_t  PSE:1;          ///< Page Size Extensions.
        uint32_t  PAE:1;          ///< Physical Address Extension.
        uint32_t  MCE:1;          ///< Machine Check Enable.
        uint32_t  PGE:1;          ///< Page Global Enable.
        uint32_t  PCE:1;          ///< Performance Monitoring Counter
        ///< Enable.
        uint32_t  OSFXSR:1;       ///< Operating System Support for
        ///< FXSAVE and FXRSTOR instructions
        uint32_t  OSXMMEXCPT:1;   ///< Operating System Support for
        ///< Unmasked SIMD Floating Point
        ///< Exceptions.
        uint32_t  UMIP:1;         ///< User-Mode Instruction Prevention.
        uint32_t  LA57:1;         ///< Linear Address 57bit.
        uint32_t  VMXE:1;         ///< VMX Enable.
        uint32_t  SMXE:1;         ///< SMX Enable.
        uint32_t  _reserved3:1;   ///< _reserved.
        uint32_t  FSGSBASE:1;     ///< FSGSBASE Enable.
        uint32_t  PCIDE:1;        ///< PCID Enable.
        uint32_t  OSXSAVE:1;      ///< XSAVE and Processor Extended States Enable.
        uint32_t  _reserved4:1;   ///< _reserved.
        uint32_t  SMEP:1;         ///< SMEP Enable.
        uint32_t  SMAP:1;         ///< SMAP Enable.
        uint32_t  PKE:1;          ///< Protection-Key Enable.
        uint32_t  _reserved5:9;   ///< _reserved.
    };
    uint64_t raw;
} IA32_CR4;

IA32_CR4 __readcr4(void);
void __writecr4(IA32_CR4 Data);

uint64_t __readcr3(void);
void __writecr3(uint64_t Data);

uint64_t __readcr2(void);
void __writecr2(uint64_t Data);

typedef union IA32_CR0 {
    struct {
        uint32_t  PE:1;           ///< Protection Enable.
        uint32_t  MP:1;           ///< Monitor Coprocessor.
        uint32_t  EM:1;           ///< Emulation.
        uint32_t  TS:1;           ///< Task Switched.
        uint32_t  ET:1;           ///< Extension Type.
        uint32_t  NE:1;           ///< Numeric Error.
        uint32_t  _reserved0:10;  ///< _reserved.
        uint32_t  WP:1;           ///< Write Protect.
        uint32_t  _reserved1:1;   ///< _reserved.
        uint32_t  AM:1;           ///< Alignment Mask.
        uint32_t  _reserved2:10;  ///< _reserved.
        uint32_t  NW:1;           ///< Mot Write-through.
        uint32_t  CD:1;           ///< Cache Disable.
        uint32_t  PG:1;           ///< Paging.
    };
    uint64_t raw;
} IA32_CR0;

IA32_CR0 __readcr0(void);
void __writecr0(IA32_CR0 Data);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MSR operations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
} PACKED IA32_EFER;

#define MSR_IA32_APIC_BASE                       0x0000001B
typedef union {
    ///
    /// Individual bit fields
    ///
    struct {
        uint32_t _reserved1:8;
        ///
        /// [Bit 8] BSP flag (R/W).
        ///
        uint32_t BSP:1;
        uint32_t _reserved2:1;
        ///
        /// [Bit 10] Enable x2APIC mode. Introduced at Display Family / Display
        /// Model 06_1AH.
        ///
        uint32_t EXTD:1;
        ///
        /// [Bit 11] APIC Global Enable (R/W).
        ///
        uint32_t EN:1;
        ///
        /// [Bits 31:12] APIC Base (R/W).
        ///
        uint32_t ApicBase:20;
        ///
        /// [Bits 63:32] APIC Base (R/W).
        ///
        uint32_t ApicBaseHi:32;
    };
    uint64_t raw;
} PACKED MSR_IA32_APIC_BASE_REGISTER;

uint64_t __rdmsr(uint32_t msr);
void __wrmsr(uint32_t msr, uint64_t Value);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Port CPUID operations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define CPUID_SIGNATURE (0x00)

void __cpuid(int __info[4], int __level);
void __cpuidex(int __info[4], int __level, int __ecx);

void cpuid(uint32_t val, uint32_t* rax, uint32_t* rbx, uint32_t* rcx, uint32_t* rdx);
void cpuidex(uint32_t val, uint64_t leaf, uint32_t* rax, uint32_t* rbx, uint32_t* rcx, uint32_t* rdx);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Misc operations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void __invlpg(uintptr_t a);

void __hlt(void);
void __nop(void);

void __ltr(uint16_t seg);
void __lgdt(gdt_t* gdt);

void __swapgs(void);

///
/// Byte packed structure for EFLAGS/RFLAGS.
/// 32-bits on IA-32.
/// 64-bits on x64.  The upper 32-bits on x64 are reserved.
///
typedef union {
    struct {
        uint32_t  CF:1;           ///< Carry Flag.
        uint32_t  always_one:1;   ///< _reserved.
        uint32_t  PF:1;           ///< Parity Flag.
        uint32_t  _reserved_1:1;   ///< _reserved.
        uint32_t  AF:1;           ///< Auxiliary Carry Flag.
        uint32_t  _reserved_2:1;   ///< _reserved.
        uint32_t  ZF:1;           ///< Zero Flag.
        uint32_t  SF:1;           ///< Sign Flag.
        uint32_t  TF:1;           ///< Trap Flag.
        uint32_t  IF:1;           ///< Interrupt Enable Flag.
        uint32_t  DF:1;           ///< Direction Flag.
        uint32_t  OF:1;           ///< Overflow Flag.
        uint32_t  IOPL:2;         ///< I/O Privilege Level.
        uint32_t  NT:1;           ///< Nested Task.
        uint32_t  _reserved_3:1;   ///< _reserved.
        uint32_t  RF:1;           ///< Resume Flag.
        uint32_t  VM:1;           ///< Virtual 8086 Mode.
        uint32_t  AC:1;           ///< Alignment Check.
        uint32_t  VIF:1;          ///< Virtual Interrupt Flag.
        uint32_t  VIP:1;          ///< Virtual Interrupt Pending.
        uint32_t  ID:1;           ///< ID Flag.
        uint32_t  _reserved_4:10;  ///< _reserved.
    };
    uint64_t raw;
} IA32_RFLAGS;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SIMD operations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef union {
    struct {
        ///
        /// [Bit 0] x87 state.
        ///
        uint32_t  x87:1;
        ///
        /// [Bit 1] SSE state.
        ///
        uint32_t  SSE:1;
        ///
        /// [Bit 2] AVX state.
        ///
        uint32_t  AVX:1;
        ///
        /// [Bits 4:3] MPX state.
        ///
        uint32_t  MPX:2;
        ///
        /// [Bits 7:5] AVX-512 state.
        ///
        uint32_t  AVX_512:3;
        ///
        /// [Bit 8] Used for IA32_XSS.
        ///
        uint32_t  IA32_XSS:1;
        ///
        /// [Bit 9] PKRU state.
        ///
        uint32_t  PKRU:1;
        uint32_t  _reserved1:3;
        ///
        /// [Bit 13] Used for IA32_XSS, part 2.
        ///
        uint32_t  IA32_XSS_2:1;
        uint32_t  _reserved2:18;
    };
    uint64_t raw;
} IA32_XCR0;

uint64_t __xgetbv(uint32_t index);
void __xsetbv(uint32_t index, uint64_t value);

typedef struct {
    uint16_t  Fcw;
    uint16_t  Fsw;
    uint16_t  Ftw;
    uint16_t  Opcode;
    uint32_t  Eip;
    uint16_t  Cs;
    uint16_t  _reserved1;
    uint32_t  DataOffset;
    uint16_t  Ds;
    uint8_t   _reserved2[2];
    uint32_t  Mxcsr;
    uint32_t  Mxcsr_Mask;
    uint8_t   St0Mm0[10];
    uint8_t   _reserved3[6];
    uint8_t   St1Mm1[10];
    uint8_t   _reserved4[6];
    uint8_t   St2Mm2[10];
    uint8_t   _reserved5[6];
    uint8_t   St3Mm3[10];
    uint8_t   _reserved6[6];
    uint8_t   St4Mm4[10];
    uint8_t   _reserved7[6];
    uint8_t   St5Mm5[10];
    uint8_t   _reserved8[6];
    uint8_t   St6Mm6[10];
    uint8_t   _reserved9[6];
    uint8_t   St7Mm7[10];
    uint8_t   _reserved10[6];
    uint8_t   Xmm0[16];
    uint8_t   Xmm1[16];
    uint8_t   Xmm2[16];
    uint8_t   Xmm3[16];
    uint8_t   Xmm4[16];
    uint8_t   Xmm5[16];
    uint8_t   Xmm6[16];
    uint8_t   Xmm7[16];
    uint8_t   Xmm8[16];
    uint8_t   Xmm9[16];
    uint8_t   Xmm10[16];
    uint8_t   Xmm11[16];
    uint8_t   Xmm12[16];
    uint8_t   Xmm13[16];
    uint8_t   Xmm14[16];
    uint8_t   Xmm15[16];
    uint8_t   _reserved11[6 * 16];
} FX_SAVE_STATE;

void __fxsave(void* state);
void __fxrstor(void* state);

void __xsave(void* state);
void __xrstor(void* state);

#endif //__TOMATOS_KERNEL_ARCH_AMD64_INTRIN_H__
