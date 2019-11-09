#ifndef TOMATKERNEL_IO_H
#define TOMATKERNEL_IO_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include <interrupts/idt.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Paging
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define PAGE_SIZE 4096
#define PAGE_MASK 0xFFFu
#define PAGE_SHIFT 12u

#define SIZE_TO_PAGES(a) (((a) >> PAGE_SHIFT) + (((a) & PAGE_MASK) ? 1u : 0u))
#define PAGES_TO_SIZE(a) ((a) << PAGE_SHIFT)

//! Vol 3A, Table 4-14. Format of an IA-32e PML4 Entry (PML4E) that References a Page-Directory-Pointer Table
typedef union _PML4E {
    uint64_t raw;
    struct {
        uint64_t present : 1;     //!< 0      Present
        uint64_t rw : 1;          //!< 1      Read/write; if 0, writes are not allowed
        uint64_t us : 1;          //!< 2      User/supervisor; if 0, user-mode access isn't allowed
        uint64_t pwt : 1;         //!< 3      Page-level write-through
        uint64_t pcd : 1;         //!< 4      Page-level cache disable
        uint64_t access : 1;      //!< 5      Accessed; indicates whether software has accessed the page
        uint64_t _ignored0 : 1;    //!< 6
        uint64_t page_size : 1;    //!< 7      Page-Size; must be 0
        uint64_t _ignored1 : 4;    //!< 8-11
        uint64_t addr : 40;       //!< 12-51  Physical address that the entry points to
        uint64_t _ignored2 : 11;   //!< 52-62
        uint64_t nx : 1;          //!< 63 If  IA32_EFER.NXE = 1, execute-disable
    };
} PML4E;
_Static_assert(sizeof(uint64_t) == sizeof(PML4E), "");

//! Vol 3A, Table 4-16. Format of an IA-32e Page-Directory-Pointer-Table Entry (PDPTE) that References a Page Directory
typedef union _PDPTE
{
    uint64_t raw;
    struct {
        uint64_t present : 1;     //!< 0      Present
        uint64_t rw : 1;          //!< 1      Read/write; if 0, writes are not allowed
        uint64_t us : 1;          //!< 2      User/supervisor; if 0, user-mode access isn't allowed
        uint64_t pwt : 1;         //!< 3      Page-level write-through
        uint64_t pcd : 1;         //!< 4      Page-level cache disable
        uint64_t access : 1;      //!< 5      Accessed; indicates whether software has accessed the page
        uint64_t dirty : 1;       //!< 6      Dirty; indicates whether software has written to the page
        uint64_t page_size : 1;    //!< 7      Page-Size; must be 0 to refernce PDE
        uint64_t _reserved1 : 4;   //!< 8-11
        uint64_t addr : 40;       //!< 12-51  Physical address that the entry points to
        uint64_t _reserved2 : 11;  //!< 52-62
        uint64_t nx : 1;          //!< 63     If IA32_EFER.NXE = 1, execute-disable
    };
} PDPTE;
_Static_assert(sizeof(uint64_t) == sizeof(PDPTE), "");

//! Vol 3A, Table 4-18. Format of an IA-32e Page-Directory Entry that References a Page Table
typedef union _PDE
{
    uint64_t raw;
    struct {
        uint64_t present : 1;     //!< 0      Present
        uint64_t rw : 1;          //!< 1      Read/write; if 0, writes are not allowed
        uint64_t us : 1;          //!< 2      User/supervisor; if 0, user-mode access isn't allowed
        uint64_t pwt : 1;         //!< 3      Page-level write-through
        uint64_t pcd : 1;         //!< 4      Page-level cache disable
        uint64_t access : 1;      //!< 5      Accessed; indicates whether software has accessed the page
        uint64_t reserved0 : 1;   //!< 6
        uint64_t page_size : 1;    //!< 7      Page-Size; must be 0 to reference PTE
        uint64_t reserved1 : 4;   //!< 8-11
        uint64_t addr : 40;       //!< 12-51  Physical address that the entry points to
        uint64_t reserved2 : 11;  //!< 52-62
        uint64_t nx : 1;          //!< 63     If IA32_EFER.NXE = 1, execute-disable
    };
} PDE;
_Static_assert(sizeof(uint64_t) == sizeof(PDE), "");

//! Vol 3A, Table 4-19. Format of an IA-32e Page-Table Entry that Maps a 4-KByte Page
typedef union _PTE
{
    uint64_t qwValue;
    struct {
        uint64_t present : 1;     //!< 0      Present
        uint64_t rw : 1;          //!< 1      Read/write; if 0, writes are not allowed
        uint64_t us : 1;          //!< 2      User/supervisor; if 0, user-mode access isn't allowed
        uint64_t pwt : 1;         //!< 3      Page-level write-through
        uint64_t pcd : 1;         //!< 4      Page-level cache disable
        uint64_t access : 1;      //!< 5      Accessed; indicates whether software has accessed the page
        uint64_t dirty : 1;       //!< 6      Dirty; indicates whether software has written to the page
        uint64_t pat : 1;         //!< 7      Page Attribute Table;
        uint64_t global : 1;      //!< 8      Global; if CR4.PGE = 1, determines whether the translation is global
        uint64_t ignored0 : 3;    //!< 9-11
        uint64_t addr : 40;       //!< 12-51  Physical address that the entry points to
        uint64_t _ignored1 : 7;    //!< 52-58
        uint64_t prot_key : 4;     //!< 59-62  Protection key; if CR4.PKE = 1, determines the protection key of the page
        uint64_t nx : 1;          //!< 63     If IA32_EFER.NXE = 1, execute-disable
    };
} PTE;
_Static_assert(sizeof(uint64_t) == sizeof(PTE), "");

typedef union _VA_ADDRESS
{
    uint64_t raw;

    //! Figure 4-8. Linear-Address Translation to a 4-KByte Page using IA-32e Paging
    struct {
        uint64_t offset : 12;     //!< 0-11
        uint64_t pte : 9;    //!< 12-20
        uint64_t pde : 9;    //!< 21-29
        uint64_t pdpte : 9;  //!< 30-38
        uint64_t pml4e : 9;  //!< 39-47
        uint64_t _reserved0 : 16;  //!< 48-63
    } four_kb;

    //! Figure 4-9. Linear-Address Translation to a 2-MByte Page using IA-32e Paging
    struct {
        uint64_t offset : 21;     //!< 0-20
        uint64_t pde : 9;    //!< 21-29
        uint64_t pdpte : 9;  //!< 30-38
        uint64_t pml4e : 9;  //!< 39-47
        uint64_t _reserved0 : 16;  //!< 48-63
    } two_mb;

    //! Figure 4-10. Linear-Address Translation to a 1-GByte Page using IA-32e Paging
    struct {
        uint64_t offset : 30;     //!< 0-29
        uint64_t pdpte : 9;  //!< 30-38
        uint64_t pml4e : 9;  //!< 39-47
        uint64_t _reserved0 : 16;  //!< 48-63
    } one_gb;
} VA_ADDRESS;
_Static_assert(sizeof(uint64_t) == sizeof(VA_ADDRESS), "");

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MSRS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define MSR_CODE_IA32_FS_BASE 0xC0000100
#define MSR_CODE_IA32_GS_BASE 0xC0000101
#define MSR_CODE_IA32_KERNEL_GS_BASE 0xC0000102

#define MSR_CODE_IA32_APIC_BASE 0x1B

typedef union _IA32_APIC_BASE
{
    uint64_t raw;
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
        uint64_t  _apic_base : 24;
        uint64_t  _reserved2 : 28;
    };
} IA32_APIC_BASE;

#define MSR_CODE_IA32_EFER 0xC0000080

typedef union _IA32_EFER
{
    uint64_t raw;
    struct {
        // Enables SYSCALL/SYSRET instructions in 64bit
        uint64_t sce : 1;
        uint64_t _reserved0 : 7;
        // Enables IA-32e mode operation
        uint64_t lme : 1;
        uint64_t _reserved1 : 1;
        // Indicates IA-32e mode is active when set
        uint64_t lma : 1;
        // Execute Disable Bit Enable
        uint64_t nxe : 1;
        uint64_t _reserved2 : 52;
    };
} IA32_EFER;
_Static_assert(sizeof(uint64_t) == sizeof(IA32_EFER), "");

#define MSR_CODE_IA32_PAT 0x277

/*
 * Vol 3A, Table 11-10. Memory Types That Can Be Encoded With PAT
 */
typedef enum _IA32_PAT_MEMTYPE {
    // Uncacheable (UC)
    IA32_PAT_MEMTYPE_UC = 0,
    // Write Combining (WC)
    IA32_PAT_MEMTYPE_WC = 1,
    // Write Through (WT)
    IA32_PAT_MEMTYPE_WT = 4,
    // Write Protected (WP)
    IA32_PAT_MEMTYPE_WP = 5,
    // Write Back (WB)
    IA32_PAT_MEMTYPE_WB = 6,
    // Uncached (UC-)
    IA32_PAT_MEMTYPE_UCM = 7,
} IA32_PAT_MEMTYPE;

// Vol 3A, Table 11-12. Memory Type Setting of PAT Entries Following a Power-up or Reset
#define PAT0_DEFAULT_MEMTYPE IA32_PAT_MEMTYPE_WB
#define PAT1_DEFAULT_MEMTYPE IA32_PAT_MEMTYPE_WT
#define PAT2_DEFAULT_MEMTYPE IA32_PAT_MEMTYPE_UCM
#define PAT3_DEFAULT_MEMTYPE IA32_PAT_MEMTYPE_UC
#define PAT4_DEFAULT_MEMTYPE IA32_PAT_MEMTYPE_WB
#define PAT5_DEFAULT_MEMTYPE IA32_PAT_MEMTYPE_WT
#define PAT6_DEFAULT_MEMTYPE IA32_PAT_MEMTYPE_UCM
#define PAT7_DEFAULT_MEMTYPE IA32_PAT_MEMTYPE_UC

/*
 * Also see, Table 11-11. Selection of PAT Entries with PAT, PCD, and PWT Flags
 */
typedef union _IA32_PAT {
    uint64_t raw;
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
    };
} IA32_PAT;
_Static_assert(sizeof(IA32_PAT) == sizeof(uint64_t), "size mismatch");

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CPUID
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Basic cpu information
#define CPUID_FUNCTION_BASIC_VENDOR 0
#define CPUID_FUNCTION_BASIC_FEATURES 1
#define CPUID_FUNCTION_BASIC_TLB 2
#define CPUID_FUNCTION_BASIC_SERIAL 3

// Structured Extended Feature Flags Enumeration Leaf
#define CPUID_FUNCTION_FEATURES_EX 7

// Time Stamp Counter/Core Crystal Clock Information-leaf
#define CPUID_FUNCTION_TSC 0x15

// Processor Frequency Information Leaf
#define CPUID_FUNCTION_FREQUENCY 0x16

// Extended Function CPUID Information
#define CPUID_FUNCTION_EX_MAXFUNC 0x80000000
#define CPUID_FUNCTION_EX_FEATURES 0x80000001

// Vol 2A, Table 3-17. Information Returned by CPUID Instruction
typedef struct _CPUID_BASIC_VENDOR
{
    uint32_t max_basic_info;
    char vendor[12];
} CPUID_BASIC_VENDOR;
_Static_assert(sizeof(uint32_t[4]) == sizeof(CPUID_BASIC_VENDOR), "");

// Vol 2A, Table 3-17. Information Returned by CPUID Instruction
// Vol 2A, Figure 3-6. Version Information Returned by CPUID in EAX
// Vol 2A, Table 3-19. Feature Information Returned in the ECX Register
// Vol 2A, Table 3-20. More on Feature Information Returned in the EDX Register
// https://en.wikipedia.org/wiki/CPUID#EAX=1:_Processor_Info_and_Feature_Bits
typedef struct _CPUID_BASIC_FEATURES
{
    uint32_t steppingId : 4;      //!< EAX 0-3    Stepping ID
    uint32_t model : 4;           //!< EAX 4-7    Model
    uint32_t familyId : 4;        //!< EAX 8-11   Family (0FH for the Pentium 4 Processor Family)
    uint32_t processorType : 2;   //!< EAX 12-13  See CPUID_PROCESSOR_TYPE
    uint32_t reserved0 : 2;       //!< EAX 14-15  0
    uint32_t exModelId : 4;       //!< EAX 16-19  Extended Model ID (0)
    uint32_t exFamilyId : 8;      //!< EAX 20-27  Extended Family ID (0)
    uint32_t reserved1 : 4;       //!< EAX 28-31  0
    uint32_t brandIndex : 8;      //!< EBX 0-7    Brand index
    uint32_t clflushLineSize : 8; //!< EBX 8-15   CLFLUSH line size (Value ? 8 = cache line size in bytes; used also by CLFLUSHOPT).
    uint32_t maxApicId : 8;       //!< EBX 16-23  Maximum number of addressable IDs for logical processors in this physical package
    uint32_t initialApicId : 8;   //!< EBX 24-31  Initial APIC ID
    uint32_t sse3 : 1;            //!< ECX 0      Prescott New Instructions-SSE3 (PNI)
    uint32_t pclmulqdq : 1;       //!< ECX 1      PCLMULQDQ support
    uint32_t dtes64 : 1;          //!< ECX 2      64-bit debug store (edx bit 21)
    uint32_t monitor : 1;         //!< ECX 3      MONITOR and MWAIT instructions (SSE3)
    uint32_t dscpl : 1;           //!< ECX 4      CPL qualified debug store
    uint32_t vmx : 1;             //!< ECX 5      Virtual Machine eXtensions
    uint32_t smx : 1;             //!< ECX 6      Safer Mode Extensions (LaGrande)
    uint32_t est : 1;             //!< ECX 7      Enhanced SpeedStep
    uint32_t tm2 : 1;             //!< ECX 8      Thermal Monitor 2
    uint32_t ssse3 : 1;           //!< ECX 9      Supplemental SSE3 instructions
    uint32_t cnxtid : 1;          //!< ECX 10     L1 Context ID
    uint32_t sdbg : 1;            //!< ECX 11     Silicon Debug interface
    uint32_t fma : 1;             //!< ECX 12     Fused multiply-add (FMA3)
    uint32_t cx16 : 1;            //!< ECX 13     CMPXCHG16B instruction
    uint32_t xtpr : 1;            //!< ECX 14     Can disable sending task priority messages
    uint32_t pdcm : 1;            //!< ECX 15     Perfmon & debug capability
    uint32_t reserved2 : 1;       //!< ECX 16     0
    uint32_t pcid : 1;            //!< ECX 17     Process context identifiers (CR4 bit 17)
    uint32_t dca : 1;             //!< ECX 18     Direct cache access for DMA writes
    uint32_t sse41 : 1;           //!< ECX 19     SSE4.1 instructions
    uint32_t sse42 : 1;           //!< ECX 20     SSE4.2 instructions
    uint32_t x2apic : 1;          //!< ECX 21     x2APIC support
    uint32_t movbe : 1;           //!< ECX 22     MOVBE instruction (big-endian)
    uint32_t popcnt : 1;          //!< ECX 23     POPCNT instruction
    uint32_t tscdeadline : 1;     //!< ECX 24     APIC supports one-shot operation using a TSC deadline value
    uint32_t aes : 1;             //!< ECX 25     AES instruction set
    uint32_t xsave : 1;           //!< ECX 26     XSAVE, XRESTOR, XSETBV, XGETBV
    uint32_t osxsave : 1;         //!< ECX 27     XSAVE enabled by OS
    uint32_t avx : 1;             //!< ECX 28     Advanced Vector Extensions
    uint32_t f16c : 1;            //!< ECX 29     F16C (half-precision) FP support
    uint32_t rdrnd : 1;           //!< ECX 30     RDRAND (on-chip random number generator) support
    uint32_t hypervisor : 1;      //!< ECX 31     Running on a hypervisor
    uint32_t fpu : 1;             //!< EDX 0      Onboard x87 FPU
    uint32_t vme : 1;             //!< EDX 1      Virtual 8086 mode extensions (such as VIF, VIP, PIV)
    uint32_t de : 1;              //!< EDX 2      Debugging extensions (CR4 bit 3)
    uint32_t pse : 1;             //!< EDX 3      Page Size Extension
    uint32_t tsc : 1;             //!< EDX 4      Time Stamp Counter
    uint32_t msr : 1;             //!< EDX 5      Model-specific registers
    uint32_t pae : 1;             //!< EDX 6      Physical Address Extension
    uint32_t mce : 1;             //!< EDX 7      Machine Check Exception
    uint32_t cx8 : 1;             //!< EDX 8      CMPXCHG8 (compare-and-swap) instruction
    uint32_t apic : 1;            //!< EDX 9      Onboard Advanced Programmable Interrupt Controller
    uint32_t reserved3 : 1;       //!< EDX 10     0
    uint32_t sep : 1;             //!< EDX 11     SYSENTER and SYSEXIT instructions
    uint32_t mtrr : 1;            //!< EDX 12     Memory Type Range Registers
    uint32_t pge : 1;             //!< EDX 13     Page Global Enable bit in CR4
    uint32_t mca : 1;             //!< EDX 14     Machine check architecture
    uint32_t cmov : 1;            //!< EDX 15     Conditional move and FCMOV instructions
    uint32_t pat : 1;             //!< EDX 16     Page Attribute Table
    uint32_t pse36 : 1;           //!< EDX 17     36-bit page size extension
    uint32_t psn : 1;             //!< EDX 18     Processor Serial Number
    uint32_t clfsh : 1;           //!< EDX 19     CLFLUSH instruction (SSE2)
    uint32_t reserved4 : 1;       //!< EDX 20     0
    uint32_t ds : 1;              //!< EDX 21     Debug store: save trace of executed jumps
    uint32_t acpi : 1;            //!< EDX 22     Onboard thermal control MSRs for ACPI
    uint32_t mmx : 1;             //!< EDX 23     MMX instructions
    uint32_t fxsr : 1;            //!< EDX 24     FXSAVE, FXRESTOR instructions, CR4 bit 9
    uint32_t sse : 1;             //!< EDX 25     SSE instructions (a.k.a. Katmai New Instructions)
    uint32_t sse2 : 1;            //!< EDX 26     SSE2 instructions
    uint32_t ss : 1;              //!< EDX 27     CPU cache supports self-snoop
    uint32_t htt : 1;             //!< EDX 28     Hyper-threading
    uint32_t tm : 1;              //!< EDX 29     Thermal monitor automatically limits temperature
    uint32_t ia64 : 1;            //!< EDX 30     IA64 processor emulating x86
    uint32_t pbe : 1;             //!< EDX 31     Pending Break Enable (PBE# pin) wakeup support
} CPUID_BASIC_FEATURES;
_Static_assert(sizeof(uint32_t[4]) == sizeof(CPUID_BASIC_FEATURES), "");

//! Vol 2A, Table 3-17. Information Returned by CPUID Instruction
// https://en.wikipedia.org/wiki/CPUID#EAX=7,_ECX=0:_Extended_Features
typedef struct _CPUID_FEATURES_EX
{
    uint32_t dwMaxExSubFunc;      //!< EAX 0-31   Reports the maximum input value for supported leaf 7 sub-leaves
    uint32_t fsgbase : 1;         //!< EBX 0      Supports RDFSBASE/RDGSBASE/WRFSBASE/WRGSBASE if 1
    uint32_t ia32TscAdjust : 1;   //!< EBX 1      IA32_TSC_ADJUST MSR is supported if 1
    uint32_t sgx : 1;             //!< EBX 2      Software Guard Extensions
    uint32_t bm1 : 1;             //!< EBX 3      Bit Manipulation Instruction Set 1
    uint32_t hle : 1;             //!< EBX 4      Transactional Synchronization Extensions
    uint32_t avx2 : 1;            //!< EBX 5      Advanced Vector Extensions 2
    uint32_t fdpExcptnOnly : 1;   //!< EBX 6      FDP_EXCPTN_ONLY. x87 FPU Data Pointer updated only on x87 exceptions if 1
    uint32_t smep : 1;            //!< EBX 7      Supervisor-Mode Execution Prevention
    uint32_t bmi2 : 1;            //!< EBX 8      Bit Manipulation Instruction Set 2
    uint32_t erms : 1;            //!< EBX 9      Enhanced REP MOVSB/STOSB
    uint32_t invcpid : 1;         //!< EBX 10     INVPCID instruction
    uint32_t rtm : 1;             //!< EBX 11     Transactional Synchronization Extensions
    uint32_t pqm : 1;             //!< EBX 12     Platform Quality of Service Monitoring
    uint32_t depFpuCsDs : 1;      //!< EBX 13     FPU CS and FPU DS deprecated
    uint32_t mpx : 1;             //!< EBX 14     Intel MPX (Memory Protection Extensions)
    uint32_t pqe : 1;             //!< EBX 15     Platform Quality of Service Enforcement
    uint32_t avx512f : 1;         //!< EBX 16     AVX-512 Foundation
    uint32_t avx512fdq : 1;       //!< EBX 17     AVX-512 DWORD and QWORD Instructions
    uint32_t rdseed : 1;          //!< EBX 18     RDSEED instruction
    uint32_t adx : 1;             //!< EBX 19     Intel ADX (Multi-Precision Add-Carry Instruction Extensions)
    uint32_t smap : 1;            //!< EBX 20     Supervisor Mode Access Prevention
    uint32_t avx512ifma : 1;      //!< EBX 21     AVX-512 Integer Fused Multiply-Add Instructions
    uint32_t pcommit : 1;         //!< EBX 22     PCOMMIT instruction
    uint32_t clflushopt : 1;      //!< EBX 23     CLFLUSHOPT instruction
    uint32_t clwb : 1;            //!< EBX 24     CLWB instruction
    uint32_t intelPt : 1;         //!< EBX 25     Intel Processor Trace
    uint32_t avx512pf : 1;        //!< EBX 26     AVX-512 Prefetch Instructions
    uint32_t avx512er : 1;        //!< EBX 27     AVX-512 Exponential and Reciprocal Instructions
    uint32_t avx512cd : 1;        //!< EBX 28     AVX-512 Conflict Detection Instructions
    uint32_t sha : 1;             //!< EBX 29     Intel SHA extensions
    uint32_t avx512bw : 1;        //!< EBX 30     AVX-512 Byte and Word Instructions
    uint32_t avx512vl : 1;        //!< EBX 31     AVX-512 Vector Length Extensions
    uint32_t prefetchwt1 : 1;     //!< ECX 0      PREFETCHWT1
    uint32_t avx512vbmi : 1;      //!< ECX 1      AVX-512 Vector Bit Manipulation Instructions
    uint32_t umip : 1;            //!< ECX 2      User-mode Instruction Prevention
    uint32_t pku : 1;             //!< ECX 3      Memory Protection Keys for User-mode pages
    uint32_t ospke : 1;           //!< ECX 4      PKU enabled by OS
    uint32_t reserved0 : 1;       //!< ECX 5      0
    uint32_t avx512vbmi2 : 1;     //!< ECX 6      AVX-512 Vector Bit Manipulation Instructions 2
    uint32_t reserved1 : 1;       //!< ECX 7      0
    uint32_t gfni : 1;            //!< ECX 8      Galois Field instructions
    uint32_t vaes : 1;            //!< ECX 9      AES instruction set (VEX-256/EVEX)
    uint32_t vpclmulqdq : 1;      //!< ECX 10     CLMUL instruction set (VEX-256/EVEX)
    uint32_t avx512vnni : 1;      //!< ECX 11     AVX-512 Vector Neural Network Instructions
    uint32_t avx512bitalg : 1;    //!< ECX 12     AVX-512 BITALG instructions
    uint32_t reserved2 : 1;       //!< ECX 13     0
    uint32_t mawau : 8;           //!< ECX 14-21  The value of userspace MPX Address-Width Adjust used by the BNDLDX and BNDSTX Intel MPX instructions in 64 - bit mode
    uint32_t rdpid : 1;           //!< ECX 22     Read Processor ID
    uint32_t reserved3 : 7;       //!< ECX 23-29  0
    uint32_t sgxLc : 1;           //!< ECX 30     SGX Launch Configuration
    uint32_t reserved4 : 1;       //!< ECX 31     0
    uint32_t reserved5 : 2;       //!< EDX 0-1    0
    uint32_t avx512vnniw : 1;     //!< EDX 2      AVX-512 4-register Neural Network Instructions
    uint32_t avx512maps : 1;      //!< EDX 2      AVX-512 4-register Multiply Accumulation Single precision
    uint32_t reserved6 : 22;      //!< EDX 4-25   0
    uint32_t specCtrl : 1;        //!< EDX 26     Speculation Control: Indirect Branch Restricted Speculation(IBRS) and Indirect Branch Prediction Barrier(IBPB)
    uint32_t reserved7 : 5;       //!< EDX 27-31  0
} CPUID_FEATURES_EX;
_Static_assert(sizeof(uint32_t[4]) == sizeof(CPUID_FEATURES_EX), "");

//! Vol 2A, Table 3-17. Information Returned by CPUID Instruction
// https://en.wikipedia.org/wiki/CPUID#EAX=80000001h:_Extended_Processor_Info_and_Feature_Bits
typedef struct _CPUID_EX_FEATURES
{
    uint32_t reserved0;           //!< EAX 0-31
    uint32_t reserved1;           //!< EBX 0-31
    uint32_t lahfLm : 1;          //!< ECX 0      LAHF/SAHF in long mode
    uint32_t cmpLegacy : 1;       //!< ECX 1      Hyper-threading not valid
    uint32_t svm : 1;             //!< ECX 2      Secure Virtual Machine
    uint32_t extApic : 1;         //!< ECX 3      Extended APIC space
    uint32_t cr8Legacy : 1;       //!< ECX 4      CR8 in 32-bit mode
    uint32_t abm : 1;             //!< ECX 5      Advanced bit manipulation (LZCNT and POPCNT)
    uint32_t sse4a : 1;           //!< ECX 6      SSE4a
    uint32_t misAlignSse : 1;     //!< ECX 7      Misaligned SSE mode
    uint32_t prefetchNow : 1;     //!< ECX 8      PREFETCH and PREFETCHW instructions
    uint32_t osvw : 1;            //!< ECX 9      OS Visible Workaround
    uint32_t ibs : 1;             //!< ECX 10     Instruction Based Sampling
    uint32_t xop : 1;             //!< ECX 11     XOP instruction set
    uint32_t skinit : 1;          //!< ECX 12     SKINIT/STGI instructions
    uint32_t wdt : 1;             //!< ECX 13     Watchdog timer
    uint32_t reserved2 : 1;       //!< ECX 14     0
    uint32_t lwp : 1;             //!< ECX 15     Light Weight Profiling
    uint32_t fma4 : 1;            //!< ECX 16     4 operands fused multiply-add
    uint32_t tce : 1;             //!< ECX 17     Translation Cache Extension
    uint32_t reserved3 : 1;       //!< ECX 18     0
    uint32_t nodeIdMsr : 1;       //!< ECX 19     NodeID MSR
    uint32_t reserved4 : 1;       //!< ECX 20     0
    uint32_t tbm : 1;             //!< ECX 21     Trailing Bit Manipulation
    uint32_t topoext : 1;         //!< ECX 22     Topology Extensions
    uint32_t perfctrCore : 1;     //!< ECX 23     Core performance counter extensions
    uint32_t perfctrNb : 1;       //!< ECX 24     NB performance counter extensions
    uint32_t reserved5 : 1;       //!< ECX 25     0
    uint32_t dbx : 1;             //!< ECX 26     Data breakpoint extensions
    uint32_t perftsc : 1;         //!< ECX 27     Performance TSC
    uint32_t pcxi2i : 1;          //!< ECX 28     L2I perf counter extensions
    uint32_t reserved6 : 3;       //!< ECX 29-31
    uint32_t fpu : 1;             //!< EDX 0      Onboard x87 FPU
    uint32_t vme : 1;             //!< EDX 1      Virtual mode extensions (VIF)
    uint32_t de : 1;              //!< EDX 2      Debugging extensions (CR4 bit 3)
    uint32_t pse : 1;             //!< EDX 3      Page Size Extension
    uint32_t tsc : 1;             //!< EDX 4      Time Stamp Counter
    uint32_t msr : 1;             //!< EDX 5      Model-specific registers
    uint32_t pae : 1;             //!< EDX 6      Physical Address Extension
    uint32_t mce : 1;             //!< EDX 7      Machine Check Exception
    uint32_t cx8 : 1;             //!< EDX 8      CMPXCHG8 (compare-and-swap) instruction
    uint32_t apic : 1;            //!< EDX 9      Onboard Advanced Programmable Interrupt Controller
    uint32_t reserved7 : 1;       //!< EDX 10     0
    uint32_t syscall : 1;         //!< EDX 11     SYSCALL and SYSRET instructions
    uint32_t mtrr : 1;            //!< EDX 12     Memory Type Range Registers
    uint32_t pge : 1;             //!< EDX 13     Page Global Enable bit in CR4
    uint32_t mca : 1;             //!< EDX 14     Machine check architecture
    uint32_t cmov : 1;            //!< EDX 15     Conditional move and FCMOV instructions
    uint32_t pat : 1;             //!< EDX 16     Page Attribute Table
    uint32_t pse36 : 1;           //!< EDX 17     36-bit page size extension
    uint32_t reserved8 : 1;       //!< EDX 18     0
    uint32_t mp : 1;              //!< EDX 19     Multiprocessor Capable
    uint32_t nx : 1;              //!< EDX 20     NX bit
    uint32_t reserved9 : 1;       //!< EDX 21     0
    uint32_t mmxext : 1;          //!< EDX 22     Extended MMX
    uint32_t mmx : 1;             //!< EDX 23     MMX instructions
    uint32_t fxsr : 1;            //!< EDX 24     FXSAVE, FXRSTOR instructions, CR4 bit 9
    uint32_t fxsropt : 1;         //!< EDX 25     FXSAVE/FXRSTOR optimizations
    uint32_t oneGbPages : 1;      //!< EDX 26     1GB pages
    uint32_t rdtscp : 1;          //!< EDX 27     RDTSCP instruction
    uint32_t reservedA : 1;       //!< EDX 28     0
    uint32_t lm : 1;              //!< EDX 29     Long mode
    uint32_t threeDNowExt : 1;    //!< EDX 30     Extended 3DNow!
    uint32_t threeDNow : 1;       //!< EDX 31     3DNow!
} CPUID_EX_FEATURES;
_Static_assert(sizeof(uint32_t[4]) == sizeof(CPUID_EX_FEATURES), "");

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CPU registers
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef union _IA32_CR0 {
    struct {
        uint32_t  PE : 1;           ///< Protection Enable.
        uint32_t  MP : 1;           ///< Monitor Coprocessor.
        uint32_t  EM : 1;           ///< Emulation.
        uint32_t  TS : 1;           ///< Task Switched.
        uint32_t  ET : 1;           ///< Extension Type.
        uint32_t  NE : 1;           ///< Numeric Error.
        uint32_t  _reserved_0 : 10;  ///< Reserved.
        uint32_t  WP : 1;           ///< Write Protect.
        uint32_t  _reserved_1 : 1;   ///< Reserved.
        uint32_t  AM : 1;           ///< Alignment Mask.
        uint32_t  _reserved_2 : 10;  ///< Reserved.
        uint32_t  NW : 1;           ///< Mot Write-through.
        uint32_t  CD : 1;           ///< Cache Disable.
        uint32_t  PG : 1;           ///< Paging.
    };
    uint64_t raw;
} IA32_CR0;

typedef union _IA32_CR4 {
    struct {
        uint32_t VME : 1;          ///< Virtual-8086 Mode Extensions.
        uint32_t PVI : 1;          ///< Protected-Mode Virtual Interrupts.
        uint32_t TSD : 1;          ///< Time Stamp Disable.
        uint32_t DE : 1;           ///< Debugging Extensions.
        uint32_t PSE : 1;          ///< Page Size Extensions.
        uint32_t PAE : 1;          ///< Physical Address Extension.
        uint32_t MCE : 1;          ///< Machine Check Enable.
        uint32_t PGE : 1;          ///< Page Global Enable.
        uint32_t PCE : 1;          ///< Performance Monitoring Counter Enable.
        uint32_t OSFXSR : 1;       ///< Operating System Support for FXSAVE and FXRSTOR instructions
        uint32_t OSXMMEXCPT : 1;   ///< Operating System Support for Unmasked SIMD Floating Point Exceptions.
        uint32_t UMIP : 1;         ///< User-Mode Instruction Prevention
        uint32_t LA57 : 1;         ///< Linear Address 57bit.
        uint32_t VMXE : 1;         ///< VMX Enable
        uint32_t SMXE : 1;         ///< Safer Mode Extensions Enable
        uint32_t FSGSBASE : 1;     ///< Enables the instructions RDFSBASE, RDGSBASE, WRFSBASE, and WRGSBASE.
        uint32_t PCIDE : 1;        ///< PCID Enable
        uint32_t OSXSAVE : 1;      ///< XSAVE and Processor Extended States Enable
        uint32_t SMEP : 1;         ///< Supervisor Mode Execution Protection Enable
        uint32_t SMAP : 1;         ///< Supervisor Mode Access Prevention Enable
        uint32_t PKE : 1;          ///< Protection Key Enable
        uint32_t _reserved_1 : 9;  ///< Reserved.
    };
    uint64_t raw;
} IA32_CR4;

typedef union _IA32_EFLAGS {
    struct {
        uint64_t CF:1;           ///< Carry Flag.
        uint64_t _reserved0:1;   ///< Reserved.
        uint64_t PF:1;           ///< Parity Flag.
        uint64_t _reserved1:1;   ///< Reserved.
        uint64_t AF:1;           ///< Auxiliary Carry Flag.
        uint64_t _reserved2:1;   ///< Reserved.
        uint64_t ZF:1;           ///< Zero Flag.
        uint64_t SF:1;           ///< Sign Flag.
        uint64_t TF:1;           ///< Trap Flag.
        uint64_t IF:1;           ///< Interrupt Enable Flag.
        uint64_t DF:1;           ///< Direction Flag.
        uint64_t OF:1;           ///< Overflow Flag.
        uint64_t IOPL:2;         ///< I/O Privilege Level.
        uint64_t NT:1;           ///< Nested Task.
        uint64_t _reserved3:1;   ///< Reserved.
        uint64_t RF:1;           ///< Resume Flag.
        uint64_t VM:1;           ///< Virtual 8086 Mode.
        uint64_t AC:1;           ///< Alignment Check.
        uint64_t VIF:1;          ///< Virtual Interrupt Flag.
        uint64_t VIP:1;          ///< Virtual Interrupt Pending.
        uint64_t ID:1;           ///< ID Flag.
        uint64_t _reserved4:10;  ///< Reserved.
    };
    uint64_t raw;
} IA32_RFLAGS;
_Static_assert(sizeof(uint64_t) == sizeof(IA32_RFLAGS), "");

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Intrinsics
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void io_write_8(uint16_t port, uint8_t data);
void io_write_16(uint16_t port, uint16_t data);
void io_write_32(uint16_t port, uint32_t data);

uint8_t io_read_8(uint16_t port);
uint16_t io_read_16(uint16_t port);
uint32_t io_read_32(uint16_t port);

uint64_t read_msr(uint32_t code);
void write_msr(uint32_t code, uint64_t value);

void read_idtr(idt_t* idtr);
void write_idtr(idt_t* idtr);

void write_tr(uint16_t seg);

uint64_t read_cr0();
void write_cr0(uint64_t value);

uint64_t read_cr2();
void write_cr2(uint64_t value);

uint64_t read_cr3();
void write_cr3(uint64_t value);

uint64_t read_cr4();
void write_cr4(uint64_t value);

// Vol 2A, Table 3-17. Information Returned by CPUID Instruction
#define CPUID_EAX   0
#define CPUID_EBX   1
#define CPUID_ECX   2
#define CPUID_EDX   3

void cpuid(int code, int subcode, uint32_t data[4]);

uint64_t read_eflags();

/**
 * Reads the current value of Time Stamp Counter (TSC).
 */
uint64_t read_tsc();

/**
 * Requests CPU to pause for a short period of time. Typically used in MP
 * systems to prevent memory starvation while waiting for a spin lock.
 */
void cpu_pause();

void enable_interrupts();
void disable_interrupts();
bool get_interrupts_state();
bool save_and_disable_interrupts();
bool set_interrupt_state(bool state);

void memory_fence();

#endif //TOMATKERNEL_IO_H
