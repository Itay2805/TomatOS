#include <logger/logger.h>
#include <smp/cpustorage.h>
#include "gdt.h"

static gdt_entries_t gdt_entries = {
    {   // null descriptor
        .limit              = 0x0000,
        .base_low           = 0x0000,
        .base_mid           = 0x00,
        .access             = 0b00000000,
        .granularity        = 0b00000000,
        .base_high          = 0x00
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
            .length         = sizeof(tss64_t),
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

static tss64_t tss64 = {
    .iopb_offset = sizeof(tss64_t)
};

/**
 * Will set the gdt segments so they will be correct with the new segments we set
 */
extern void gdt_fix_segments();

error_t per_cpu_gdt_and_tss_init() {
    // set the tss address
    gdt.entries->tss.low = (uint16_t) (((uint64_t) &tss64) & 0xFFFF);
    gdt.entries->tss.mid = (uint8_t) (((uint64_t) &tss64 >> 16) & 0xFF);
    gdt.entries->tss.high = (uint8_t) (((uint64_t) &tss64 >> 24) & 0xFF);
    gdt.entries->tss.upper32 = (uint32_t) (((uint64_t) &tss64 >> 32) & 0xFFFFFFFF);

    // use the per cpu kernel stack
    tss64.rsp0 = cpu_kernel_stack;
    tss64.rsp1 = cpu_kernel_stack;
    tss64.rsp2 = cpu_kernel_stack;

    // TODO: Maybe have a different value for these (?)
    tss64.ist1 = cpu_kernel_stack;
    tss64.ist2 = cpu_kernel_stack;
    tss64.ist3 = cpu_kernel_stack;
    tss64.ist4 = cpu_kernel_stack;
    tss64.ist5 = cpu_kernel_stack;
    tss64.ist6 = cpu_kernel_stack;
    tss64.ist7 = cpu_kernel_stack;

    // set the gdt and tss
    log_info("\tloading new gdt and tss");
    _lgdt(gdt);
    _ltr(GDT_TSS);

    // fix the segments
    gdt_fix_segments();

    return NO_ERROR;
}