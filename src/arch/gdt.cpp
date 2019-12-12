#include "gdt.hpp"

#include <stdint.h>

namespace arch::gdt {

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Internal struct definitions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    struct __attribute__((packed)) gdt64_entry {
        uint16_t limit;
        uint16_t base_low;
        uint8_t base_mid;
        uint8_t access;
        uint8_t granularity;
        uint8_t base_high;
    };

    struct __attribute__((packed)) tss64_entry {
        uint16_t length;
        uint16_t low;
        uint8_t mid;
        uint8_t flags1;
        uint8_t flags2;
        uint8_t high;
        uint32_t upper32;
        uint32_t reserved;
    };

    struct __attribute__((packed)) gdt_entries {
        gdt64_entry null;
        gdt64_entry kernel_code;
        gdt64_entry kernel_data;
        gdt64_entry user_data;
        gdt64_entry user_code;
        tss64_entry tss;
    };

    struct __attribute__((packed)) gdt {
        uint16_t size;
        gdt_entries *entries;
    };

    struct __attribute__((packed)) tss64 {
        uint32_t reserved_1;
        uint64_t rsp0;
        uint64_t rsp1;
        uint64_t rsp2;
        uint64_t reserved_2;
        uint64_t ist1;
        uint64_t ist2;
        uint64_t ist3;
        uint64_t ist4;
        uint64_t ist5;
        uint64_t ist6;
        uint64_t ist7;
        uint64_t reserved_3;
        uint32_t iopb_offset;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // The gdt declaration
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    static gdt_entries gdt_entries = {
            {   // null descriptor
                .limit          = 0x0000,
                .base_low       = 0x0000,
                .base_mid       = 0x00,
                .access         = 0b00000000,
                .granularity    = 0b00000000,
                .base_high      = 0x00
            },
            {   // kernel code
                .limit          = 0x0000,
                .base_low       = 0x0000,
                .base_mid       = 0x00,
                .access         = 0b10011010,
                .granularity    = 0b00100000,
                .base_high      = 0x00
            },
            {   // kernel data
                .limit          = 0x0000,
                .base_low       = 0x0000,
                .base_mid       = 0x00,
                .access         = 0b10010010,
                .granularity    = 0b00000000,
                .base_high      = 0x00
            },
            {   // user data
                .limit          = 0x0000,
                .base_low       = 0x0000,
                .base_mid       = 0x00,
                .access         = 0b11110010,
                .granularity    = 0b00000000,
                .base_high      = 0x00
            },
            {   // user code
                .limit          = 0x0000,
                .base_low       = 0x0000,
                .base_mid       = 0x00,
                .access         = 0b11111010,
                .granularity    = 0b00100000,
                .base_high      = 0x00
            },
            {   // TSS
                .length         = 0,
                // Will be filled by the init function
                .low            = 0,
                .mid            = 0,
                .high           = 0,
                .upper32        = 0,
                .flags1         = 0b10001001,
                .flags2         = 0b00000000,
                .reserved       = 0
            }
    };

    // the gdt descriptor, have this as extern c so we can easily access
    // it from assembly
    extern "C" gdt gdt = {
        .size = sizeof(gdt_entries),
        .entries = &gdt_entries
    };
}
