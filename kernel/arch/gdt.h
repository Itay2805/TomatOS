#ifndef TOMATOS_GDT_H
#define TOMATOS_GDT_H

#include <util/defs.h>

#define GDT_KERNEL_CODE offsetof(gdt_entries_t, kernel_code)
#define GDT_KERNEL_DATA offsetof(gdt_entries_t, kernel_data)
#define GDT_USER_DATA   (offsetof(gdt_entries_t, user_data) | 3)
#define GDT_USER_CODE   (offsetof(gdt_entries_t, user_code) | 3)
#define GDT_TSS         (offsetof(gdt_entries_t, tss) | 3)

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
    gdt64_entry_t user_data;
    gdt64_entry_t user_code;
    tss64_entry_t tss;
} PACKED gdt_entries_t;

typedef struct gdt {
    uint16_t size;
    gdt_entries_t* entries;
} PACKED gdt_t;

typedef struct tss64 {
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
} PACKED tss64_t;

extern gdt_t g_gdt;

void init_gdt();

#endif //TOMATOS_GDT_H