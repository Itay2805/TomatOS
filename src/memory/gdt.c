#include <stdint.h>
#include <smp/percpu_storage.h>
#include <util/arch.h>

#include "gdt.h"

static gdt_entries_t gdt_entries = {
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

// we want this none-static since we are going to use it in the tboot stub
gdt_t gdt = {
    .size = sizeof(gdt_entries),
    .entries = &gdt_entries
};

void tss_init() {
    uintptr_t tss = (uintptr_t) &get_percpu_storage()->tss;

    gdt.entries->tss.length = sizeof(tss64_t);
    gdt.entries->tss.low = (uint16_t) (tss & 0xFFFF);
    gdt.entries->tss.mid = (uint8_t) ((tss >> 16) & 0xFF);
    gdt.entries->tss.high = (uint8_t) ((tss >> 24) & 0xFF);
    gdt.entries->tss.upper32 = (uint32_t) ((tss >> 32) & 0xFFFFFFFF);
    gdt.entries->tss.flags1 = 0b10001001;
    gdt.entries->tss.flags2 = 0b00000000;

    write_tr((uint16_t)GDT_TSS);
}
