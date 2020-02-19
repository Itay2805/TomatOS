#include <smp/pcpu.h>
#include "gdt.h"
#include "stack_allocator.h"

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

CPU_LOCAL tss64_t tss;

// we want this none-static since we are going to use it in the tboot stub
gdt_t gdt = {
        .size = sizeof(gdt_entries),
        .entries = &gdt_entries
};

void init_tss_for_cpu() {
    // setup the interrupt stack
    tss.rsp0 = allocate_stack();
    tss.iopb_offset = sizeof(tss64_t);

    // setup the stacks for different interrupts
    tss.ist1 = allocate_stack(); // exceptions
    tss.ist2 = allocate_stack(); // nmi
    tss.ist3 = allocate_stack(); // page faults

    // setup the tss entry in the gdt
    uintptr_t tss_base = get_pcpu_base() + (uintptr_t)&tss;
    gdt.entries->tss.length = sizeof(tss64_t);
    gdt.entries->tss.low = (uint16_t) (tss_base & 0xFFFFu);
    gdt.entries->tss.mid = (uint8_t) ((tss_base >> 16u) & 0xFFu);
    gdt.entries->tss.high = (uint8_t) ((tss_base >> 24u) & 0xFFu);
    gdt.entries->tss.upper32 = (uint32_t) ((tss_base >> 32u) & 0xFFFFFFFFu);
    gdt.entries->tss.flags1 = 0b10001001;
    gdt.entries->tss.flags2 = 0b00000000;


}
