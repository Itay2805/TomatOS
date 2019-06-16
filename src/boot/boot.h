#ifndef TOMATKERNEL_BOOT_H
#define TOMATKERNEL_BOOT_H

#include <stdint.h>
#include <stddef.h>

typedef enum mmap_type {
    MMAP_AVAILABLE,
    MMAP_RESERVED,
    MMAP_ACPI_RELAIMABLE,
    MMAP_ACPI_NVS,
    MMAP_BAD_MEMORY,
    MMAP_MMIO
} mmap_type_t;

typedef struct mmap_entry {
    uint64_t addr;
    uint64_t size;
    mmap_type_t type;
} mmap_entry_t;

typedef struct mmap {
    size_t count;
    mmap_entry_t* entries;
} mmap_t;

typedef enum framebuffer_type {
    FRAMEBUFFER_NOT_SUPPORTED,
    FRAMEBUFFER_TEXT_MODE,
    FRAMEBUFFER_GRAPHICS_MODE,
} framebuffer_type_t;

typedef struct framebuffer {
    framebuffer_type_t type;
    uint64_t addr;
    size_t width;
    size_t height;
    size_t bpp;
} framebuffer_t;

typedef struct boot_info {
    // memory
    uint64_t total_mem;
    mmap_t mmap;

    // for drivers
    void* rsdp_ptr;
    framebuffer_t framebuffer;
} boot_info_t;

extern void kernel_main(boot_info_t* info);

#endif