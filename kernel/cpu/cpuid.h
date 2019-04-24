#ifndef TOMATKERNEL_CPUID_H
#define TOMATKERNEL_CPUID_H

#include <common/stdint.h>

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

typedef struct cpu_features {
    union {
        struct {
            uint64_t fpu : 1;
            uint64_t vme : 1;
            uint64_t de : 1;
            uint64_t pse : 1;
            uint64_t tsc : 1;
            uint64_t msr : 1;
            uint64_t pae : 1;
            uint64_t mce : 1;
            uint64_t cx8 : 1;
            uint64_t apic : 1;
            uint64_t sep : 1;
            uint64_t mtrr : 1;
            uint64_t pge : 1;
            uint64_t mca : 1;
            uint64_t cmov : 1;
            uint64_t pat : 1;
            uint64_t pse36 : 1;
            uint64_t psn : 1;
            uint64_t clf : 1;
            uint64_t dtes : 1;
            uint64_t acpi : 1;
            uint64_t mmx : 1;
            uint64_t fxsr : 1;
            uint64_t sse : 1;
            uint64_t see2 : 1;
            uint64_t ss : 1;
            uint64_t htt : 1;
            uint64_t tm1 : 1;
            uint64_t ia64 : 1;
            uint64_t pbe : 1;
            uint64_t sse3 : 1;
            uint64_t pclmul : 1;
            uint64_t dtes64 : 1;
            uint64_t monitor : 1;
            uint64_t ds_cpl : 1;
            uint64_t vmx : 1;
            uint64_t smx : 1;
            uint64_t est : 1;
            uint64_t tm2 : 1;
            uint64_t ssse3 : 1;
            uint64_t cid : 1;
            uint64_t fma : 1;
            uint64_t cx16 : 1;
            uint64_t etprd : 1;
            uint64_t pdcm : 1;
            uint64_t pcide : 1;
            uint64_t dca : 1;
            uint64_t sse4_1 : 1;
            uint64_t see4_2 : 1;
            uint64_t x2apic : 1;
            uint64_t movbe : 1;
            uint64_t popcnt : 1;
            uint64_t aes : 1;
            uint64_t xsave : 1;
            uint64_t osxsave : 1;
            uint64_t avx : 1;
        };
        struct {
            uint32_t edx;
            uint32_t ecx;
        };
    };
} cpu_features_t;

static inline void cpuid(int code, uint32_t data[4]) {
    asm volatile("cpuid":"=a"(*data),"=b"(*(data+1)), "=c"(*(data+2)),"=d"(*(data+3)):"a"(code));
}


/**
 * Will get the cpu features
 */
static inline void get_cpu_features(cpu_features_t* features) {
    uint32_t data[4] = {0};
    cpuid(CPUID_GETFEATURES, data);
    features->ecx = data[2];
    features->ecx = data[3];
}

/**
 * Get the vendor of the cpu
 *
 * must be 12bytes long
 */
static inline void get_cpu_vendor(int* vendor_id, const char* vendor_str) {
    uint32_t data[4] = {0};
    cpuid(CPUID_GETVENDORSTRING, data);
    ((uint32_t*)vendor_str)[0] = data[1];
    ((uint32_t*)vendor_str)[1] = data[3];
    ((uint32_t*)vendor_str)[2] = data[2];
    *vendor_id = data[0];
}

#endif //TOMATKERNEL_CPUID_H
