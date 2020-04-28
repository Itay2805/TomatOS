#ifndef __MM_GDT_H__
#define __MM_GDT_H__

#include <stdint.h>
#include <stddef.h>

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
} __attribute__((packed)) gdt64_entry_t;

typedef struct tss64_entry {
    uint16_t length;
    uint16_t low;
    uint8_t mid;
    uint8_t flags1;
    uint8_t flags2;
    uint8_t high;
    uint32_t upper32;
    uint32_t reserved;
} __attribute__((packed)) tss64_entry_t;

typedef struct gdt_entries {
    gdt64_entry_t null;
    gdt64_entry_t kernel_code;
    gdt64_entry_t kernel_data;
    gdt64_entry_t user_data;
    gdt64_entry_t user_code;
    tss64_entry_t tss;
} __attribute__((packed)) gdt_entries_t;

typedef struct gdt {
    uint16_t size;
    gdt_entries_t* entries;
} __attribute__((packed)) gdt_t;

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
} __attribute__((packed)) tss64_t;

/**
 * The gdt itself
 *
 * we use one gdt for all cores
 */
extern gdt_t gdt;

/**
 * Load the gdt for the current cpu
 * will also fix the segments
 */
void init_gdt_for_bsp();

/**
 * Load the TSS for the current cpu
 */
void init_tss_for_cpu();

#endif //__MM_GDT_H__
