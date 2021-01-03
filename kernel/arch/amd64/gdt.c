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
    "movw %%ax, %%fs\n"
    "movw %%ax, %%gs\n"
    ::: "memory", "rax");
}
