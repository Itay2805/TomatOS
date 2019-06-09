#ifndef TOMATKERNEL_CPUID_H
#define TOMATKERNEL_CPUID_H

#include <stdint.h>

////////////////////////////////////////////////////////////////////////////
// CPUID
////////////////////////////////////////////////////////////////////////////
//
// This allows to easily check for cpu features and get the vendor
//
////////////////////////////////////////////////////////////////////////////


#define CPUID_GETVENDORSTRING   0
#define CPUID_GETFEATURES       1
#define CPUID_GETTLB            2
#define CPUID_GETSERIAL         3

#define CPUID_EAX   0
#define CPUID_EBX   1
#define CPUID_ECX   2
#define CPUID_EDX   3

typedef struct cpu_features {
        /*** EDX ***/
        union {
            struct {
                // Onboard x87 FPU
                uint32_t fpu : 1;
                // Virtual 8086 mode extensions (such as VIF, VIP, PIV)
                uint32_t vme : 1;
                // Debugging extensions (CR4 bit 3)
                uint32_t de : 1;
                // Page Size Extension
                uint32_t pse : 1;
                // Time Stamp Counter
                uint32_t tsc : 1;
                // Model-specific registers
                uint32_t msr : 1;
                // Physical Address Extension
                uint32_t pae : 1;
                // Machine Check Exception
                uint32_t mce : 1;
                // CMPXCHG8 (compare-and-swap) instruction
                uint32_t cx8 : 1;
                // Onboard Advanced Programmable Interrupt Controller
                uint32_t apic : 1;
                uint32_t _reserved0 : 1;
                // SYSENTER and SYSEXIT instructions
                uint32_t sep : 1;
                // Memory Type Range Registers
                uint32_t mtrr : 1;
                // Page Global Enable bit in CR4
                uint32_t pge : 1;
                // Machine check architecture
                uint32_t mca : 1;
                // Conditional move and FCMOV instructions
                uint32_t cmov : 1;
                // Page Attribute Table
                uint32_t pat : 1;
                // 36-bit page size extension
                uint32_t pse36 : 1;
                // Processor Serial Number
                uint32_t psn : 1;
                // CLFLUSH instruction (SSE2)
                uint32_t clfsh : 1;
                uint32_t _reserved1 : 1;
                //Debug store: save trace of executed jumps
                uint32_t ds : 1;
                // Onboard thermal control MSRs for ACPI
                uint32_t acpi : 1;
                // MMX instructions
                uint32_t mmx : 1;
                // FXSAVE, FXRESTOR instructions, CR4 bit 9
                uint32_t fxsr : 1;
                // SSE instructions (a.k.a. Katmai New Instructions)
                uint32_t sse : 1;
                // SSE2 instructions
                uint32_t sse2 : 1;
                // CPU cache implements self-snoop
                uint32_t ss : 1;
                // Hyper-threading
                uint32_t htt : 1;
                // Thermal monitor automatically limits temperature
                uint32_t tm : 1;
                // IA64 processor emulating x86
                uint32_t ia64 : 1;
                // Pending Break Enable (PBE# pin) wakeup capability
                uint32_t pbe : 1;
            };
            uint32_t edx;
        };

        /*** ECX ***/
        union {
            struct {
                // Prescott New Instructions-SSE3 (PNI)
                uint32_t sse3 : 1;
                // PCLMULQDQ
                uint32_t pclmulqdq : 1;
                // 64-bit debug store (edx bit 21)
                uint32_t dtes64 : 1;
                // MONITOR and MWAIT instructions (SSE3)
                uint32_t monitor : 1;
                // CPL qualified debug store
                uint32_t ds_cpl	 : 1;
                // Virtual Machine eXtensions
                uint32_t vmx : 1;
                // Safer Mode Extensions (LaGrande)
                uint32_t smx : 1;
                // Enhanced SpeedStep
                uint32_t est : 1;
                // Thermal Monitor 2
                uint32_t tm2 : 1;
                // Supplemental SSE3 instructions
                uint32_t ssse3 : 1;
                // L1 Context ID
                uint32_t cnxt_id : 1;
                // Silicon Debug interface
                uint32_t sdbg : 1;
                // Fused multiply-add (FMA3)
                uint32_t fma : 1;
                // CMPXCHG16B instruction
                uint32_t cx16 : 1;
                // Can disable sending task priority messages
                uint32_t xtpr : 1;
                // Perfmon & debug capability
                uint32_t pdcm : 1;
                uint32_t _reserved3 : 1;
                // Process context identifiers (CR4 bit 17)
                uint32_t pcid : 1;
                // Direct cache access for DMA writes
                uint32_t dca : 1;
                // SSE4.1 instructions
                uint32_t sse41 : 1;
                // SSE4.2 instructions
                uint32_t sse42 : 1;
                // x2APIC
                uint32_t x2apic : 1;
                // MOVBE instruction (big-endian)
                uint32_t movbe : 1;
                // POPCNT instruction
                uint32_t popcnt : 1;
                // APIC implements one-shot operation using a TSC deadline value
                uint32_t tsc_deadline : 1;
                // AES instruction set
                uint32_t aes : 1;
                // XSAVE, XRESTOR, XSETBV, XGETBV
                uint32_t xsave : 1;
                // XSAVE enabled by OS
                uint32_t osxsave : 1;
                // Advanced Vector Extensions
                uint32_t avx : 1;
                // F16C (half-precision) FP feature
                uint32_t f16c : 1;
                // RDRAND (on-chip random number generator) feature
                uint32_t rdrnd : 1;
                // Hypervisor present (always zero on physical CPUs)
                uint32_t hypervisor : 1;
            };
            uint32_t ecx;
        };
} cpu_features_t;

static inline void _cpuid(int code, int subcode, uint32_t data[4]) {
    asm volatile("cpuid"
            : "=a"(data[CPUID_EAX])
            , "=b"(data[CPUID_EBX])
            , "=c"(data[CPUID_ECX])
            , "=d"(data[CPUID_EDX])

            : "a"(code)
            , "c"(subcode)
    );
}

/**
 * Will get the cpu features
 */
static inline void get_cpu_features(cpu_features_t* features) {
    uint32_t data[4] = {0};
    _cpuid(CPUID_GETFEATURES, 0, data);
    features->ecx = data[CPUID_ECX];
    features->edx = data[CPUID_EDX];
}

/**
 * Get the vendor of the cpu
 *
 * must be 12bytes long
 */
static inline void get_cpu_vendor(int* vendor_id, const char* vendor_str) {
    uint32_t data[4] = {0};
    _cpuid(CPUID_GETVENDORSTRING, 0, data);
    ((uint32_t*)vendor_str)[0] = data[CPUID_EBX];
    ((uint32_t*)vendor_str)[1] = data[CPUID_ECX];
    ((uint32_t*)vendor_str)[2] = data[CPUID_EDX];
    *vendor_id = data[CPUID_EAX];
}

#endif //TOMATKERNEL_CPUID_H