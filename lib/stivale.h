#ifndef __STIVALE_H__
#define __STIVALE_H__

#include <stdint.h>

typedef struct stivale_header {
    uint64_t stack;
    // Flags
    // bit 0   0 = text mode,   1 = graphics mode
    uint16_t flags;
    uint16_t framebuffer_width;
    uint16_t framebuffer_height;
    uint16_t framebuffer_bpp;
} __attribute__((packed)) stivale_header_t;

typedef struct stivale_module {
    uint64_t begin;
    uint64_t end;
    char     string[128];
    uint64_t next;
} __attribute__((packed)) stivale_module_t;

typedef struct stivale_struct {
    uint64_t cmdline;
    uint64_t memory_map_addr;
    uint64_t memory_map_entries;
    uint64_t framebuffer_addr;
    uint16_t framebuffer_pitch;
    uint16_t framebuffer_width;
    uint16_t framebuffer_height;
    uint16_t framebuffer_bpp;
    uint64_t rsdp;
    uint64_t module_count;
    uint64_t modules;
} __attribute__((packed)) stivale_struct_t;

#define E820_TYPE_USABLE            (1)
#define E820_TYPE_RESERVED          (2)
#define E820_TYPE_ACPI_RECLAIM      (3)
#define E820_TYPE_ACPI_NVS          (4)
#define E820_TYPE_BAD_MEMORY        (5)

typedef struct e820_entry {
    uint64_t base;
    uint64_t length;
    uint32_t type;
    uint32_t unused;
} __attribute__((packed)) e820_entry_t;

#endif //__STIVALE_H__