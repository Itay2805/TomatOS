#ifndef __TOMATOS_KERNEL_ARCH_AMD64_STIVALE_H__
#define __TOMATOS_KERNEL_ARCH_AMD64_STIVALE_H__

#include <stdint.h>

typedef struct stivale_header {
    uint64_t stack;
    uint16_t flags;
      // bit 0  0 = text mode, 1 = graphics framebuffer mode
      // bit 1  0 = 4-level paging, 1 = use 5-level paging (if available)
      // bit 2  0 = Disable KASLR, 1 = enable KASLR (up to 1GB slide)
      // All other bits undefined.
    uint16_t framebuffer_width;
    uint16_t framebuffer_height;
    uint16_t framebuffer_bpp;
    uint64_t entry_point;
} __attribute__((packed)) stivale_header_t;

typedef struct mmap_entry {
    uint64_t base;
    uint64_t length;
    uint32_t type;
    uint32_t unused;
} __attribute__((packed)) mmap_entry_t;

typedef struct stivale_module {
    uint64_t begin;
    uint64_t end;
    char     string[128];
    struct stivale_module* next;
} __attribute__((packed)) stivale_module_t;

typedef struct stivale_struct {
    const char* cmdline;
    mmap_entry_t* memory_map_addr;
    uint64_t memory_map_entries;
    uint64_t framebuffer_addr;
    uint16_t framebuffer_pitch;
    uint16_t framebuffer_width;
    uint16_t framebuffer_height;
    uint16_t framebuffer_bpp;
    uint64_t rsdp;
    uint64_t module_count;
    stivale_module_t* modules;
    uint64_t epoch;
    uint64_t flags;
      // bit 0: 1 if booted with BIOS, 0 if booted with UEFI
      // All other bits undefined.
} __attribute__((packed)) stivale_struct_t;

#endif //__TOMATOS_KERNEL_ARCH_AMD64_STIVALE_H__
