#ifndef TOMATOS_STIVALE2_H
#define TOMATOS_STIVALE2_H

#include <util/defs.h>
#include <stdint.h>

typedef struct stivale2_tag {
    uint64_t identifier;
    struct stivale2_tag* next;
} PACKED stivale2_tag_t;

typedef struct stivale2_header {
    uint64_t entry_point;
    uint64_t stack;
    uint64_t flags;
    stivale2_tag_t* tags;
} PACKED stivale2_header_t;

#define STIVALE2_HEADER_TAG_FRAMEBUFFER_IDENT 0x3ecc1bc43d0f7971
typedef struct stivale2_header_tag_framebuffer {
    stivale2_tag_t tag;
    uint16_t width;
    uint16_t height;
    uint16_t bpp;
} PACKED stivale2_header_tag_framebuffer_t;

#define STIVALE2_HEADER_TAG_5LEVEL_PAGING_IDENT 0x932f477032007e8f

#define STIVALE2_HEADER_TAG_SMP_IDENT 0x1ab015085f3273df
typedef struct stivale2_header_tag_smp {
    stivale2_tag_t tag;
    uint64_t flags;
#define STIVALE2_HEADER_TAG_SMP_FLAG_X2APIC BIT0
} PACKED stivale2_header_tag_smp_t;

typedef struct stivale2_struct {
    char bootloader_brand[64];
    char bootloader_version[64];
    stivale2_tag_t* tags;
} PACKED stivale2_struct_t;

#define STIVALE2_STRUCT_TAG_CMDLINE_IDENT 0xe5e76a1b4597a781
typedef struct stivale2_struct_tag_cmdline {
    stivale2_tag_t tag;
    const char* cmdline;
} PACKED stivale2_struct_tag_cmdline_t;

typedef struct stivale2_mmap_entry {
    uint64_t base;
    uint64_t length;
    uint32_t type;
#define STIVALE2_MMAP_TYPE_USABLE                   1
#define STIVALE2_MMAP_TYPE_RESERVED                 2
#define STIVALE2_MMAP_TYPE_ACPI_RECLAIMABLE         3
#define STIVALE2_MMAP_TYPE_ACPI_NVS                 4
#define STIVALE2_MMAP_TYPE_BAD_MEMORY               5
#define STIVALE2_MMAP_TYPE_BOOTLOADER_RECLAIMABLE   0x1000
#define STIVALE2_MMAP_TYPE_KERNEL_AND_MODULES       0x1001
    uint32_t unused;
} PACKED stivale2_mmap_entry_t;

#define STIVALE2_STRUCT_TAG_MEMMAP_IDENT 0x2187f79e8612de07
typedef struct stivale2_struct_tag_memmap {
    stivale2_tag_t tag;
    uint64_t entries;
    stivale2_mmap_entry_t memmap[];
} PACKED stivale2_struct_tag_memmap_t;

// TODO: module

#define STIVALE2_STRUCT_TAG_RSDP_IDENT 0x9e1786930a375e78
typedef struct stivale2_struct_tag_rsdp {
    stivale2_tag_t tag;
    uint64_t rsdp;
} PACKED stivale2_struct_tag_rsdp_t;

typedef struct stivale2_smp_info {
    uint32_t acpi_processor_uid;
    uint32_t lapic_id;
    uint64_t target_stack;
    uint64_t goto_address;
    uint64_t extra_argument;
} PACKED stivale2_smp_info_t;

#define STIVALE2_STRUCT_TAG_SMP_IDENT 0x34d1d96339647025
typedef struct stivale2_struct_tag_smp {
    stivale2_tag_t tag;
    uint64_t flags;
#define STIVALE2_STRUCT_TAG_SMP_FLAG_X2APIC BIT0
    uint32_t bsp_lapic_id;
    uint32_t unused;
    uint64_t cpu_count;
    stivale2_smp_info_t smp_info[];
} PACKED stivale2_struct_tag_smp_t;

extern stivale2_struct_t* g_stivale2_struct;

/**
 * Get a stivale tag by its id, return null if not found
 */
void* get_stivale2_tag(uint64_t id);

/**
 * Convert the stivale struct to have direct map (higher half) addresses
 */
void stivale2_to_higher_half();

#endif //TOMATOS_STIVALE2_H
