#include <stdint.h>
#include <sync/lock.h>
#include <mem/mm.h>

#include "gdt.h"
#include "intrin.h"

static gdt_entries_t g_entries = {
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

gdt_t g_gdt = {
    .size = sizeof(gdt_entries_t) - 1,
    .entries = &g_entries
};

void init_gdt() {
    __lgdt(&g_gdt);

    asm volatile (
        "movq %%rsp, %%rax\n"
        "pushq $16\n"
        "pushq %%rax\n"
        "pushfq\n"
        "pushq $8\n"
        "pushq $1f\n"
        "iretq\n"
        "1:\n"
        "movw $16, %%ax\n"
        "movw %%ax, %%ds\n"
        "movw %%ax, %%es\n"
        "movw $0x1b, %%ax\n"
        "movw %%ax, %%fs\n"
        "movw %%ax, %%gs\n"
    ::: "memory", "rax");
}

/**
 * Make sure only one person edits the gdt at a time
 */
static ticket_lock_t g_tss_lock;

void init_tss() {
    tss64_t* tss = (tss64_t*)kalloc(sizeof(tss64_t));
    tss->rsp0 = (uintptr_t)alloc_stack();
    tss->iopb_offset = sizeof(tss64_t);
    tss->ist1 = (uintptr_t)alloc_stack(); // Exceptions
    tss->ist2 = (uintptr_t)alloc_stack(); // NMI
    tss->ist3 = (uintptr_t)alloc_stack(); // page fault, gp fault, double fault
    uintptr_t tss_base = (uintptr_t)tss;
    
    ticket_lock(&g_tss_lock);
    g_gdt.entries->tss.length = sizeof(tss64_t);
    g_gdt.entries->tss.low = (uint16_t) (tss_base & 0xFFFFu);
    g_gdt.entries->tss.mid = (uint8_t) ((tss_base >> 16u) & 0xFFu);
    g_gdt.entries->tss.high = (uint8_t) ((tss_base >> 24u) & 0xFFu);
    g_gdt.entries->tss.upper32 = (uint32_t) ((tss_base >> 32u) & 0xFFFFFFFFu);
    g_gdt.entries->tss.flags1 = 0b10001001;
    g_gdt.entries->tss.flags2 = 0b00000000;
    __ltr(GDT_TSS);
    ticket_unlock(&g_tss_lock);
}
