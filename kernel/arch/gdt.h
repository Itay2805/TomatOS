#ifndef TOMATOS_GDT_H
#define TOMATOS_GDT_H

#include <util/defs.h>

#define GDT_KERNEL_CODE offsetof(gdt_entries_t, kernel_code)
#define GDT_KERNEL_DATA offsetof(gdt_entries_t, kernel_data)

typedef struct gdt64_entry {
    uint16_t limit;
    uint16_t base_low;
    uint8_t base_mid;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} PACKED gdt64_entry_t;

typedef struct tss64_entry {
    uint16_t length;
    uint16_t low;
    uint8_t mid;
    uint8_t flags1;
    uint8_t flags2;
    uint8_t high;
    uint32_t upper32;
    uint32_t reserved;
} PACKED tss64_entry_t;

typedef struct gdt_entries {
    gdt64_entry_t null;
    gdt64_entry_t kernel_code;
    gdt64_entry_t kernel_data;
} PACKED gdt_entries_t;

typedef struct gdt {
    uint16_t size;
    gdt_entries_t* entries;
} PACKED gdt_t;

extern gdt_t g_gdt;

void init_gdt();

#endif //TOMATOS_GDT_H
