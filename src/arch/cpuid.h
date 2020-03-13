#ifndef __ARCH_CPUID_H__
#define __ARCH_CPUID_H__

#include <stdint.h>


/**
  CPUID Version Information
  @param   EAX  CPUID_VERSION_INFO (0x01)
  @retval  EAX  Returns Model, Family, Stepping Information described by the
                type CPUID_VERSION_INFO_EAX.
  @retval  EBX  Returns Brand, Cache Line Size, and Initial APIC ID described by
                the type CPUID_VERSION_INFO_EBX.
  @retval  ECX  CPU Feature Information described by the type
                CPUID_VERSION_INFO_ECX.
  @retval  EDX  CPU Feature Information described by the type
                CPUID_VERSION_INFO_EDX.
  <b>Example usage</b>
  @code
  CPUID_VERSION_INFO_EAX  Eax;
  CPUID_VERSION_INFO_EBX  Ebx;
  CPUID_VERSION_INFO_ECX  Ecx;
  CPUID_VERSION_INFO_EDX  Edx;
  AsmCpuid (CPUID_VERSION_INFO, &Eax.Uint32, &Ebx.Uint32, &Ecx.Uint32, &Edx.Uint32);
  @endcode
**/
#define CPUID_VERSION_INFO                      0x01

/**
  CPUID Version Information returned in EAX for CPUID leaf
  #CPUID_VERSION_INFO.
**/
typedef union {
    ///
    /// Individual bit fields
    ///
    struct {
        uint32_t  SteppingId:4;        ///< [Bits   3:0] Stepping ID
        uint32_t  Model:4;             ///< [Bits   7:4] Model
        uint32_t  FamilyId:4;          ///< [Bits  11:8] Family
        uint32_t  ProcessorType:2;     ///< [Bits 13:12] Processor Type
        uint32_t  _reserved1:2;         ///< [Bits 15:14] _reserved
        uint32_t  ExtendedModelId:4;   ///< [Bits 19:16] Extended Model ID
        uint32_t  ExtendedFamilyId:8;  ///< [Bits 27:20] Extended Family ID
        uint32_t  _reserved2:4;         ///< _reserved
    };
    ///
    /// All bit fields as a 32-bit value
    ///
    uint32_t raw;
} CPUID_VERSION_INFO_EAX;

///
/// @{ Define value for bit field CPUID_VERSION_INFO_EAX.ProcessorType
///
#define CPUID_VERSION_INFO_EAX_PROCESSOR_TYPE_ORIGINAL_OEM_PROCESSOR     0x00
#define CPUID_VERSION_INFO_EAX_PROCESSOR_TYPE_INTEL_OVERDRIVE_PROCESSOR  0x01
#define CPUID_VERSION_INFO_EAX_PROCESSOR_TYPE_DUAL_PROCESSOR             0x02
///
/// @}
///

/**
  CPUID Version Information returned in EBX for CPUID leaf
  #CPUID_VERSION_INFO.
**/
typedef union {
    ///
    /// Individual bit fields
    ///
    struct {
        ///
        /// [Bits 7:0] Provides an entry into a brand string table that contains
        /// brand strings for IA-32 processors.
        ///
        uint32_t  BrandIndex:8;
        ///
        /// [Bits 15:8] Indicates the size of the cache line flushed by the CLFLUSH
        /// and CLFLUSHOPT instructions in 8-byte increments. This field was
        /// introduced in the Pentium 4 processor.
        ///
        uint32_t  CacheLineSize:8;
        ///
        /// [Bits 23:16] Maximum number of addressable IDs for logical processors
        /// in this physical package.
        ///
        /// @note
        /// The nearest power-of-2 integer that is not smaller than EBX[23:16] is
        /// the number of unique initial APICIDs reserved for addressing different
        /// logical processors in a physical package. This field is only valid if
        /// CPUID.1.EDX.HTT[bit 28]= 1.
        ///
        uint32_t  MaximumAddressableIdsForLogicalProcessors:8;
        ///
        /// [Bits 31:24] The 8-bit ID that is assigned to the local APIC on the
        /// processor during power up. This field was introduced in the Pentium 4
        /// processor.
        ///
        uint32_t  InitialLocalApicId:8;
    };
    ///
    /// All bit fields as a 32-bit value
    ///
    uint32_t raw;
} CPUID_VERSION_INFO_EBX;

/**
  CPUID Version Information returned in ECX for CPUID leaf
  #CPUID_VERSION_INFO.
**/
typedef union {
    ///
    /// Individual bit fields
    ///
    struct {
        ///
        /// [Bit 0] Streaming SIMD Extensions 3 (SSE3).  A value of 1 indicates the
        /// processor supports this technology
        ///
        uint32_t  SSE3:1;
        ///
        /// [Bit 1] A value of 1 indicates the processor supports the PCLMULQDQ
        /// instruction.  Carryless Multiplication
        ///
        uint32_t  PCLMULQDQ:1;
        ///
        /// [Bit 2] 64-bit DS Area.  A value of 1 indicates the processor supports
        /// DS area using 64-bit layout.
        ///
        uint32_t  DTES64:1;
        ///
        /// [Bit 3] MONITOR/MWAIT.  A value of 1 indicates the processor supports
        /// this feature.
        ///
        uint32_t  MONITOR:1;
        ///
        /// [Bit 4] CPL Qualified Debug Store.  A value of 1 indicates the processor
        /// supports the extensions to the Debug Store feature to allow for branch
        /// message storage qualified by CPL
        ///
        uint32_t  DS_CPL:1;
        ///
        /// [Bit 5] Virtual Machine Extensions.  A value of 1 indicates that the
        /// processor supports this technology.
        ///
        uint32_t  VMX:1;
        ///
        /// [Bit 6] Safer Mode Extensions. A value of 1 indicates that the processor
        /// supports this technology
        ///
        uint32_t  SMX:1;
        ///
        /// [Bit 7] Enhanced Intel SpeedStep(R) technology.  A value of 1 indicates
        /// that the processor supports this technology
        ///
        uint32_t  EIST:1;
        ///
        /// [Bit 8] Thermal Monitor 2.  A value of 1 indicates whether the processor
        /// supports this technology
        ///
        uint32_t  TM2:1;
        ///
        /// [Bit 9] A value of 1 indicates the presence of the Supplemental Streaming
        /// SIMD Extensions 3 (SSSE3). A value of 0 indicates the instruction
        /// extensions are not present in the processor.
        ///
        uint32_t  SSSE3:1;
        ///
        /// [Bit 10] L1 Context ID.  A value of 1 indicates the L1 data cache mode
        /// can be set to either adaptive mode or shared mode. A value of 0 indicates
        /// this feature is not supported. See definition of the IA32_MISC_ENABLE MSR
        /// Bit 24 (L1 Data Cache Context Mode) for details
        ///
        uint32_t  CNXT_ID:1;
        ///
        /// [Bit 11] A value of 1 indicates the processor supports IA32_DEBUG_INTERFACE
        /// MSR for silicon debug
        ///
        uint32_t  SDBG:1;
        ///
        /// [Bit 12] A value of 1 indicates the processor supports FMA (Fused Multiple
        ///  Add) extensions using YMM state.
        ///
        uint32_t  FMA:1;
        ///
        /// [Bit 13] CMPXCHG16B Available.  A value of 1 indicates that the feature
        /// is available.
        ///
        uint32_t  CMPXCHG16B:1;
        ///
        /// [Bit 14] xTPR Update Control.  A value of 1 indicates that the processor
        /// supports changing IA32_MISC_ENABLE[Bit 23].
        ///
        uint32_t  xTPR_Update_Control:1;
        ///
        /// [Bit 15] Perfmon and Debug Capability:  A value of 1 indicates the
        /// processor supports the performance and debug feature indication MSR
        /// IA32_PERF_CAPABILITIES.
        ///
        uint32_t  PDCM:1;
        uint32_t  _reserved:1;
        ///
        /// [Bit 17] Process-context identifiers.  A value of 1 indicates that the
        /// processor supports PCIDs and that software may set CR4.PCIDE to 1.
        ///
        uint32_t  PCID:1;
        ///
        /// [Bit 18] A value of 1 indicates the processor supports the ability to
        /// prefetch data from a memory mapped device.  Direct Cache Access.
        ///
        uint32_t  DCA:1;
        ///
        /// [Bit 19] A value of 1 indicates that the processor supports SSE4.1.
        ///
        uint32_t  SSE4_1:1;
        ///
        /// [Bit 20] A value of 1 indicates that the processor supports SSE4.2.
        ///
        uint32_t  SSE4_2:1;
        ///
        /// [Bit 21] A value of 1 indicates that the processor supports x2APIC
        /// feature.
        ///
        uint32_t  x2APIC:1;
        ///
        /// [Bit 22] A value of 1 indicates that the processor supports MOVBE
        /// instruction.
        ///
        uint32_t  MOVBE:1;
        ///
        /// [Bit 23] A value of 1 indicates that the processor supports the POPCNT
        /// instruction.
        ///
        uint32_t  POPCNT:1;
        ///
        /// [Bit 24] A value of 1 indicates that the processor's local APIC timer
        /// supports one-shot operation using a TSC deadline value.
        ///
        uint32_t  TSC_Deadline:1;
        ///
        /// [Bit 25] A value of 1 indicates that the processor supports the AESNI
        /// instruction extensions.
        ///
        uint32_t  AESNI:1;
        ///
        /// [Bit 26] A value of 1 indicates that the processor supports the
        /// XSAVE/XRSTOR processor extended states feature, the XSETBV/XGETBV
        /// instructions, and XCR0.
        ///
        uint32_t  XSAVE:1;
        ///
        /// [Bit 27] A value of 1 indicates that the OS has set CR4.OSXSAVE[Bit 18]
        /// to enable XSETBV/XGETBV instructions to access XCR0 and to support
        /// processor extended state management using XSAVE/XRSTOR.
        ///
        uint32_t  OSXSAVE:1;
        ///
        /// [Bit 28] A value of 1 indicates the processor supports the AVX instruction
        /// extensions.
        ///
        uint32_t  AVX:1;
        ///
        /// [Bit 29] A value of 1 indicates that processor supports 16-bit
        /// floating-point conversion instructions.
        ///
        uint32_t  F16C:1;
        ///
        /// [Bit 30] A value of 1 indicates that processor supports RDRAND instruction.
        ///
        uint32_t  RDRAND:1;
        ///
        /// [Bit 31] Always returns 0.
        ///
        uint32_t  NotUsed:1;
    };
    ///
    /// All bit fields as a 32-bit value
    ///
    uint32_t raw;
} CPUID_VERSION_INFO_ECX;

/**
  CPUID Version Information returned in EDX for CPUID leaf
  #CPUID_VERSION_INFO.
**/
typedef union {
    ///
    /// Individual bit fields
    ///
    struct {
        ///
        /// [Bit 0] Floating Point Unit On-Chip. The processor contains an x87 FPU.
        ///
        uint32_t  FPU:1;
        ///
        /// [Bit 1] Virtual 8086 Mode Enhancements.  Virtual 8086 mode enhancements,
        /// including CR4.VME for controlling the feature, CR4.PVI for protected
        /// mode virtual interrupts, software interrupt indirection, expansion of
        /// the TSS with the software indirection bitmap, and EFLAGS.VIF and
        /// EFLAGS.VIP flags.
        ///
        uint32_t  VME:1;
        ///
        /// [Bit 2] Debugging Extensions.  Support for I/O breakpoints, including
        /// CR4.DE for controlling the feature, and optional trapping of accesses to
        /// DR4 and DR5.
        ///
        uint32_t  DE:1;
        ///
        /// [Bit 3] Page Size Extension.  Large pages of size 4 MByte are supported,
        /// including CR4.PSE for controlling the feature, the defined dirty bit in
        /// PDE (Page Directory Entries), optional reserved bit trapping in CR3,
        /// PDEs, and PTEs.
        ///
        uint32_t  PSE:1;
        ///
        /// [Bit 4] Time Stamp Counter.  The RDTSC instruction is supported,
        /// including CR4.TSD for controlling privilege.
        ///
        uint32_t  TSC:1;
        ///
        /// [Bit 5] Model Specific Registers RDMSR and WRMSR Instructions.  The
        /// RDMSR and WRMSR instructions are supported. Some of the MSRs are
        /// implementation dependent.
        ///
        uint32_t  MSR:1;
        ///
        /// [Bit 6] Physical Address Extension.  Physical addresses greater than 32
        /// bits are supported: extended page table entry formats, an extra level in
        /// the page translation tables is defined, 2-MByte pages are supported
        /// instead of 4 Mbyte pages if PAE bit is 1.
        ///
        uint32_t  PAE:1;
        ///
        /// [Bit 7] Machine Check Exception.  Exception 18 is defined for Machine
        /// Checks, including CR4.MCE for controlling the feature. This feature does
        /// not define the model-specific implementations of machine-check error
        /// logging, reporting, and processor shutdowns. Machine Check exception
        /// handlers may have to depend on processor version to do model specific
        /// processing of the exception, or test for the presence of the Machine
        /// Check feature.
        ///
        uint32_t  MCE:1;
        ///
        /// [Bit 8] CMPXCHG8B Instruction.  The compare-and-exchange 8 bytes(64 bits)
        /// instruction is supported (implicitly locked and atomic).
        ///
        uint32_t  CX8:1;
        ///
        /// [Bit 9] APIC On-Chip.  The processor contains an Advanced Programmable
        /// Interrupt Controller (APIC), responding to memory mapped commands in the
        /// physical address range FFFE0000H to FFFE0FFFH (by default - some
        /// processors permit the APIC to be relocated).
        ///
        uint32_t  APIC:1;
        uint32_t  _reserved1:1;
        ///
        /// [Bit 11] SYSENTER and SYSEXIT Instructions.  The SYSENTER and SYSEXIT
        /// and associated MSRs are supported.
        ///
        uint32_t  SEP:1;
        ///
        /// [Bit 12] Memory Type Range Registers.  MTRRs are supported. The MTRRcap
        /// MSR contains feature bits that describe what memory types are supported,
        /// how many variable MTRRs are supported, and whether fixed MTRRs are
        /// supported.
        ///
        uint32_t  MTRR:1;
        ///
        /// [Bit 13] Page Global Bit.  The global bit is supported in paging-structure
        /// entries that map a page, indicating TLB entries that are common to
        /// different processes and need not be flushed. The CR4.PGE bit controls
        /// this feature.
        ///
        uint32_t  PGE:1;
        ///
        /// [Bit 14] Machine Check Architecture. A value of 1 indicates the Machine
        /// Check Architecture of reporting machine errors is supported. The MCG_CAP
        /// MSR contains feature bits describing how many banks of error reporting
        /// MSRs are supported.
        ///
        uint32_t  MCA:1;
        ///
        /// [Bit 15] Conditional Move Instructions.  The conditional move instruction
        /// CMOV is supported. In addition, if x87 FPU is present as indicated by the
        /// CPUID.FPU feature bit, then the FCOMI and FCMOV instructions are supported.
        ///
        uint32_t  CMOV:1;
        ///
        /// [Bit 16] Page Attribute Table.  Page Attribute Table is supported. This
        /// feature augments the Memory Type Range Registers (MTRRs), allowing an
        /// operating system to specify attributes of memory accessed through a
        /// linear address on a 4KB granularity.
        ///
        uint32_t  PAT:1;
        ///
        /// [Bit 17] 36-Bit Page Size Extension.  4-MByte pages addressing physical
        /// memory beyond 4 GBytes are supported with 32-bit paging. This feature
        /// indicates that upper bits of the physical address of a 4-MByte page are
        /// encoded in bits 20:13 of the page-directory entry. Such physical
        /// addresses are limited by MAXPHYADDR and may be up to 40 bits in size.
        ///
        uint32_t  PSE_36:1;
        ///
        /// [Bit 18] Processor Serial Number.  The processor supports the 96-bit
        /// processor identification number feature and the feature is enabled.
        ///
        uint32_t  PSN:1;
        ///
        /// [Bit 19] CLFLUSH Instruction.  CLFLUSH Instruction is supported.
        ///
        uint32_t  CLFSH:1;
        uint32_t  _reserved2:1;
        ///
        /// [Bit 21] Debug Store.  The processor supports the ability to write debug
        /// information into a memory resident buffer.  This feature is used by the
        /// branch trace store (BTS) and precise event-based sampling (PEBS)
        /// facilities.
        ///
        uint32_t  DS:1;
        ///
        /// [Bit 22] Thermal Monitor and Software Controlled Clock Facilities.  The
        /// processor implements internal MSRs that allow processor temperature to
        /// be monitored and processor performance to be modulated in predefined
        /// duty cycles under software control.
        ///
        uint32_t  ACPI:1;
        ///
        /// [Bit 23] Intel MMX Technology.  The processor supports the Intel MMX
        /// technology.
        ///
        uint32_t  MMX:1;
        ///
        /// [Bit 24] FXSAVE and FXRSTOR Instructions.  The FXSAVE and FXRSTOR
        /// instructions are supported for fast save and restore of the floating
        /// point context. Presence of this bit also indicates that CR4.OSFXSR is
        /// available for an operating system to indicate that it supports the
        /// FXSAVE and FXRSTOR instructions.
        ///
        uint32_t  FXSR:1;
        ///
        /// [Bit 25] SSE.  The processor supports the SSE extensions.
        ///
        uint32_t  SSE:1;
        ///
        /// [Bit 26] SSE2.  The processor supports the SSE2 extensions.
        ///
        uint32_t  SSE2:1;
        ///
        /// [Bit 27] Self Snoop.  The processor supports the management of
        /// conflicting memory types by performing a snoop of its own cache
        /// structure for transactions issued to the bus.
        ///
        uint32_t  SS:1;
        ///
        /// [Bit 28] Max APIC IDs reserved field is Valid.  A value of 0 for HTT
        /// indicates there is only a single logical processor in the package and
        /// software should assume only a single APIC ID is reserved. A value of 1
        /// for HTT indicates the value in CPUID.1.EBX[23:16] (the Maximum number of
        /// addressable IDs for logical processors in this package) is valid for the
        /// package.
        ///
        uint32_t  HTT:1;
        ///
        /// [Bit 29] Thermal Monitor.  The processor implements the thermal monitor
        /// automatic thermal control circuitry (TCC).
        ///
        uint32_t  TM:1;
        uint32_t  _reserved3:1;
        ///
        /// [Bit 31] Pending Break Enable.  The processor supports the use of the
        /// FERR#/PBE# pin when the processor is in the stop-clock state (STPCLK# is
        /// asserted) to signal the processor that an interrupt is pending and that
        /// the processor should return to normal operation to handle the interrupt.
        /// Bit 10 (PBE enable) in the IA32_MISC_ENABLE MSR enables this capability.
        ///
        uint32_t  PBE:1;
    };
    ///
    /// All bit fields as a 32-bit value
    ///
    uint32_t  raw;
} CPUID_VERSION_INFO_EDX;

/**
  CPUID Extended Processor Signature and Feature Bits
  @param   EAX  CPUID_EXTENDED_CPU_SIG (0x80000001)
  @retval  EAX  CPUID_EXTENDED_CPU_SIG.
  @retval  EBX  _reserved.
  @retval  ECX  Extended Processor Signature and Feature Bits information
                described by the type CPUID_EXTENDED_CPU_SIG_ECX.
  @retval  EDX  Extended Processor Signature and Feature Bits information
                described by the type CPUID_EXTENDED_CPU_SIG_EDX.
  <b>Example usage</b>
  @code
  uint32_t                      Eax;
  CPUID_EXTENDED_CPU_SIG_ECX  Ecx;
  CPUID_EXTENDED_CPU_SIG_EDX  Edx;
  AsmCpuid (CPUID_EXTENDED_CPU_SIG, &Eax, NULL, &Ecx.Uint32, &Edx.Uint32);
  @endcode
**/
#define CPUID_EXTENDED_CPU_SIG                  0x80000001

/**
  CPUID Extended Processor Signature and Feature Bits ECX for CPUID leaf
  #CPUID_EXTENDED_CPU_SIG.
**/
typedef union {
    ///
    /// Individual bit fields
    ///
    struct {
        ///
        /// [Bit 0] LAHF/SAHF available in 64-bit mode.
        ///
        uint32_t  LAHF_SAHF:1;
        uint32_t  __reserved1:4;
        ///
        /// [Bit 5] LZCNT.
        ///
        uint32_t  LZCNT:1;
        uint32_t  __reserved2:2;
        ///
        /// [Bit 8] PREFETCHW.
        ///
        uint32_t  PREFETCHW:1;
        uint32_t  __reserved3:23;
    };
    ///
    /// All bit fields as a 32-bit value
    ///
    uint32_t Uint32;
} CPUID_EXTENDED_CPU_SIG_ECX;

typedef union {
    ///
    /// Individual bit fields
    ///
    struct {
        uint32_t __reserved1:11;
        ///
        /// [Bit 11] SYSCALL/SYSRET available in 64-bit mode.
        ///
        uint32_t  SYSCALL_SYSRET:1;
        uint32_t  __reserved2:8;
        ///
        /// [Bit 20] Execute Disable Bit available.
        ///
        uint32_t  NX:1;
        uint32_t  __reserved3:5;
        ///
        /// [Bit 26] 1-GByte pages are available if 1.
        ///
        uint32_t  Page1GB:1;
        ///
        /// [Bit 27] RDTSCP and IA32_TSC_AUX are available if 1.
        ///
        uint32_t  RDTSCP:1;
        uint32_t  __reserved4:1;
        ///
        /// [Bit 29] Intel(R) 64 Architecture available if 1.
        ///
        uint32_t  LM:1;
        uint32_t  __reserved5:2;
    };
    ///
    /// All bit fields as a 32-bit value
    ///
    uint32_t raw;
} CPUID_EXTENDED_CPU_SIG_EDX;

#define CPUID_STRUCTURED_EXTENDED_FEATURE_FLAGS                0x07

///
/// CPUID Structured Extended Feature Flags Enumeration sub-leaf
///
#define CPUID_STRUCTURED_EXTENDED_FEATURE_FLAGS_SUB_LEAF_INFO  0x00

/**
  CPUID Structured Extended Feature Flags Enumeration in EBX for CPUID leaf
  #CPUID_STRUCTURED_EXTENDED_FEATURE_FLAGS sub leaf
  #CPUID_STRUCTURED_EXTENDED_FEATURE_FLAGS_SUB_LEAF_INFO.
**/
typedef union {
    ///
    /// Individual bit fields
    ///
    struct {
        ///
        /// [Bit 0] Supports RDFSBASE/RDGSBASE/WRFSBASE/WRGSBASE if 1.
        ///
        uint32_t  FSGSBASE:1;
        ///
        /// [Bit 1] IA32_TSC_ADJUST MSR is supported if 1.
        ///
        uint32_t  IA32_TSC_ADJUST:1;
        ///
        /// [Bit 2] Intel SGX is supported if 1. See section 37.7 "DISCOVERING SUPPORT
        /// FOR INTEL(R) SGX AND ENABLING ENCLAVE INSTRUCTIONS".
        ///
        uint32_t  SGX:1;
        ///
        /// [Bit 3] If 1 indicates the processor supports the first group of advanced
        /// bit manipulation extensions (ANDN, BEXTR, BLSI, BLSMSK, BLSR, TZCNT)
        ///
        uint32_t  BMI1:1;
        ///
        /// [Bit 4] Hardware Lock Elision
        ///
        uint32_t  HLE:1;
        ///
        /// [Bit 5] If 1 indicates the processor supports AVX2 instruction extensions.
        ///
        uint32_t  AVX2:1;
        ///
        /// [Bit 6] x87 FPU Data Pointer updated only on x87 exceptions if 1.
        ///
        uint32_t  FDP_EXCPTN_ONLY:1;
        ///
        /// [Bit 7] Supports Supervisor-Mode Execution Prevention if 1.
        ///
        uint32_t  SMEP:1;
        ///
        /// [Bit 8] If 1 indicates the processor supports the second group of
        /// advanced bit manipulation extensions (BZHI, MULX, PDEP, PEXT, RORX,
        /// SARX, SHLX, SHRX)
        ///
        uint32_t  BMI2:1;
        ///
        /// [Bit 9] Supports Enhanced REP MOVSB/STOSB if 1.
        ///
        uint32_t  EnhancedRepMovsbStosb:1;
        ///
        /// [Bit 10] If 1, supports INVPCID instruction for system software that
        /// manages process-context identifiers.
        ///
        uint32_t  INVPCID:1;
        ///
        /// [Bit 11] Restricted Transactional Memory
        ///
        uint32_t  RTM:1;
        ///
        /// [Bit 12] Supports Intel(R) Resource Director Technology (Intel(R) RDT)
        /// Monitoring capability if 1.
        ///
        uint32_t  RDT_M:1;
        ///
        /// [Bit 13] Deprecates FPU CS and FPU DS values if 1.
        ///
        uint32_t  DeprecateFpuCsDs:1;
        ///
        /// [Bit 14] Supports Intel(R) Memory Protection Extensions if 1.
        ///
        uint32_t  MPX:1;
        ///
        /// [Bit 15] Supports Intel(R) Resource Director Technology (Intel(R) RDT)
        /// Allocation capability if 1.
        ///
        uint32_t  RDT_A:1;
        ///
        /// [Bit 16] AVX512F.
        ///
        uint32_t  AVX512F:1;
        ///
        /// [Bit 17] AVX512DQ.
        ///
        uint32_t  AVX512DQ:1;
        ///
        /// [Bit 18] If 1 indicates the processor supports the RDSEED instruction.
        ///
        uint32_t  RDSEED:1;
        ///
        /// [Bit 19] If 1 indicates the processor supports the ADCX and ADOX
        /// instructions.
        ///
        uint32_t  ADX:1;
        ///
        /// [Bit 20] Supports Supervisor-Mode Access Prevention (and the CLAC/STAC
        /// instructions) if 1.
        ///
        uint32_t  SMAP:1;
        ///
        /// [Bit 21] AVX512_IFMA.
        ///
        uint32_t  AVX512_IFMA:1;
        uint32_t  _reserved6:1;
        ///
        /// [Bit 23] If 1 indicates the processor supports the CLFLUSHOPT instruction.
        ///
        uint32_t  CLFLUSHOPT:1;
        ///
        /// [Bit 24] If 1 indicates the processor supports the CLWB instruction.
        ///
        uint32_t  CLWB:1;
        ///
        /// [Bit 25] If 1 indicates the processor supports the Intel Processor Trace
        /// extensions.
        ///
        uint32_t  IntelProcessorTrace:1;
        ///
        /// [Bit 26] AVX512PF. (Intel Xeon Phi only.).
        ///
        uint32_t  AVX512PF:1;
        ///
        /// [Bit 27] AVX512ER. (Intel Xeon Phi only.).
        ///
        uint32_t  AVX512ER:1;
        ///
        /// [Bit 28] AVX512CD.
        ///
        uint32_t  AVX512CD:1;
        ///
        /// [Bit 29] Supports Intel(R) Secure Hash Algorithm Extensions (Intel(R)
        /// SHA Extensions) if 1.
        ///
        uint32_t  SHA:1;
        ///
        /// [Bit 30] AVX512BW.
        ///
        uint32_t  AVX512BW:1;
        ///
        /// [Bit 31] AVX512VL.
        ///
        uint32_t  AVX512VL:1;
    };
    ///
    /// All bit fields as a 32-bit value
    ///
    uint32_t  raw;
} CPUID_STRUCTURED_EXTENDED_FEATURE_FLAGS_EBX;

/**
  CPUID Structured Extended Feature Flags Enumeration in ECX for CPUID leaf
  #CPUID_STRUCTURED_EXTENDED_FEATURE_FLAGS sub leaf
  #CPUID_STRUCTURED_EXTENDED_FEATURE_FLAGS_SUB_LEAF_INFO.
**/
typedef union {
    ///
    /// Individual bit fields
    ///
    struct {
        ///
        /// [Bit 0] If 1 indicates the processor supports the PREFETCHWT1 instruction.
        /// (Intel Xeon Phi only.)
        ///
        uint32_t  PREFETCHWT1:1;
        ///
        /// [Bit 1] AVX512_VBMI.
        ///
        uint32_t  AVX512_VBMI:1;
        ///
        /// [Bit 2] Supports user-mode instruction prevention if 1.
        ///
        uint32_t  UMIP:1;
        ///
        /// [Bit 3] Supports protection keys for user-mode pages if 1.
        ///
        uint32_t  PKU:1;
        ///
        /// [Bit 4] If 1, OS has set CR4.PKE to enable protection keys (and the
        /// RDPKRU/WRPKRU instructions).
        ///
        uint32_t  OSPKE:1;
        uint32_t  _reserved5:9;
        ///
        /// [Bits 14] AVX512_VPOPCNTDQ. (Intel Xeon Phi only.).
        ///
        uint32_t  AVX512_VPOPCNTDQ:1;
        uint32_t  _reserved7:1;
        ///
        /// [Bits 16] Supports 5-level paging if 1.
        ///
        uint32_t  FiveLevelPage:1;
        ///
        /// [Bits 21:17] The value of MAWAU used by the BNDLDX and BNDSTX instructions
        /// in 64-bit mode.
        ///
        uint32_t  MAWAU:5;
        ///
        /// [Bit 22] RDPID and IA32_TSC_AUX are available if 1.
        ///
        uint32_t  RDPID:1;
        uint32_t  _reserved3:7;
        ///
        /// [Bit 30] Supports SGX Launch Configuration if 1.
        ///
        uint32_t  SGX_LC:1;
        uint32_t  _reserved4:1;
    };
    ///
    /// All bit fields as a 32-bit value
    ///
    uint32_t  raw;
} CPUID_STRUCTURED_EXTENDED_FEATURE_FLAGS_ECX;

/**
  CPUID Structured Extended Feature Flags Enumeration in EDX for CPUID leaf
  #CPUID_STRUCTURED_EXTENDED_FEATURE_FLAGS sub leaf
  #CPUID_STRUCTURED_EXTENDED_FEATURE_FLAGS_SUB_LEAF_INFO.
**/
typedef union {
    ///
    /// Individual bit fields
    ///
    struct {
        ///
        /// [Bit 1:0] _reserved.
        ///
        uint32_t  _reserved1:2;
        ///
        /// [Bit 2] AVX512_4VNNIW. (Intel Xeon Phi only.)
        ///
        uint32_t  AVX512_4VNNIW:1;
        ///
        /// [Bit 3] AVX512_4FMAPS. (Intel Xeon Phi only.)
        ///
        uint32_t  AVX512_4FMAPS:1;
        ///
        /// [Bit 25:4] _reserved.
        ///
        uint32_t  _reserved2:22;
        ///
        /// [Bit 26] Enumerates support for indirect branch restricted speculation
        /// (IBRS) and the indirect branch pre-dictor barrier (IBPB). Processors
        /// that set this bit support the IA32_SPEC_CTRL MSR and the IA32_PRED_CMD
        /// MSR. They allow software to set IA32_SPEC_CTRL[0] (IBRS) and
        /// IA32_PRED_CMD[0] (IBPB).
        ///
        uint32_t  EnumeratesSupportForIBRSAndIBPB:1;
        ///
        /// [Bit 27] Enumerates support for single thread indirect branch
        /// predictors (STIBP). Processors that set this bit support the
        /// IA32_SPEC_CTRL MSR. They allow software to set IA32_SPEC_CTRL[1]
        /// (STIBP).
        ///
        uint32_t  EnumeratesSupportForSTIBP:1;
        ///
        /// [Bit 28] Enumerates support for L1D_FLUSH. Processors that set this bit
        /// support the IA32_FLUSH_CMD MSR. They allow software to set
        /// IA32_FLUSH_CMD[0] (L1D_FLUSH).
        ///
        uint32_t  EnumeratesSupportForL1D_FLUSH:1;
        ///
        /// [Bit 29] Enumerates support for the IA32_ARCH_CAPABILITIES MSR.
        ///
        uint32_t  EnumeratesSupportForCapability:1;
        ///
        /// [Bit 30] _reserved.
        ///
        uint32_t  _reserved3:1;
        ///
        /// [Bit 31] Enumerates support for Speculative Store Bypass Disable (SSBD).
        /// Processors that set this bit sup-port the IA32_SPEC_CTRL MSR. They allow
        /// software to set IA32_SPEC_CTRL[2] (SSBD).
        ///
        uint32_t  EnumeratesSupportForSSBD:1;
    };
    ///
    /// All bit fields as a 32-bit value
    ///
    uint32_t  raw;
} CPUID_STRUCTURED_EXTENDED_FEATURE_FLAGS_EDX;

void cpuid(uint32_t val, uint32_t* rax, uint32_t* rbx, uint32_t* rcx, uint32_t* rdx);
void cpuidex(uint32_t val, uint64_t leaf, uint32_t* rax, uint32_t* rbx, uint32_t* rcx, uint32_t* rdx);

#endif //__ARCH_CPUID_H__
