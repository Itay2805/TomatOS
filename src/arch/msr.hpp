#pragma once

#include <stdint.h>

namespace arch {

     constexpr int MSR_CODE_IA32_PAT = 0x277;

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
    static_assert(sizeof(IA32_PAT) == sizeof(uint64_t));

    uint64_t read_msr(uint32_t code);
    void write_msr(uint32_t code, uint64_t value);

}
