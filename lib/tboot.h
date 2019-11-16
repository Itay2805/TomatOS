#ifndef TOMATBOOT_UEFI_TBOOT_H
#define TOMATBOOT_UEFI_TBOOT_H

#include <stddef.h>

/*
 * The magic passed as a parameter to the kernel
 */
#define TBOOT_MAGIC                     0xCAFEBABE

/*
 * Usually contains uefi runtime services code or
 */
#define TBOOT_MEMORY_TYPE_RESERVED      0

/*
 * Memory that contains errors and is not to be used
 */
#define TBOOT_MEMORY_TYPE_BAD_MEMORY    1

/*
 * This memory is to be preserved by the UEFI OS loader and OS until
 * ACPI is enabled. Once ACPI is enabled, the memory in this range is
 * available for general use.
 */
#define TBOOT_MEMORY_TYPE_ACPI_RECLAIM  2

/*
 * Memory available for general use
 */
#define TBOOT_MEMORY_TYPE_USABLE        3

/*
 * This memory is to be preserved by the UEFI OS loader and OS in the
 * working and ACPI S1–S3 states.
 */
#define TBOOT_MEMORY_TYPE_ACPI_NVS      4

/*
 * This region in memory contains boot info related data
 * it can be allocated once you are done with the boot
 * information
 */
#define TBOOT_MEMORY_TYPE_BOOT_INFO     5

/*
 * This region in memory contains the kernel itself,
 * you should treat it as reserved probably
 */
#define TBOOT_MEMORY_TYPE_KERNEL        6

/*
 * Entry in the memory map, the type is one of
 * the TBOOT_MEMORY_TYPE_*
 */
typedef struct tboot_mmap_entry {
    uint32_t type;
    uint64_t addr;
    uint64_t len;
} __attribute__((packed)) tboot_mmap_entry_t;

typedef struct tboot_info {
    /**
     * The memory map, contains information about the memory
     */
    struct {
        uint64_t count;
        tboot_mmap_entry_t* entries;
    } __attribute__((packed)) mmap;

    /**
     * The framebuffer, it is in BGRX format
     */
    struct {
        uint64_t addr;
        uint32_t width;
        uint32_t height;
    } __attribute__((packed)) framebuffer;

    /**
     * The command line arguments, passed from the
     * configuration file of the loaded entry
     */
    struct {
        uint32_t length;
        char* cmdline;
    } __attribute__((packed)) cmdline;

    /**
     * The frequency (ticks per second) of the TSC
     *
     * @remark
     * Note that the TSC freq may not be constant, so don't
     * rely on it too much after the initial boot stage
     * or on other cores.
     */
    uint64_t tsc_freq;

    /**
     * Pointer to the RSDP table, may be 0 if the
     * table was not found
     */
    uint64_t rsdp;
} __attribute__((packed)) tboot_info_t;

/**
 * This is the signature of the kernel entry
 */
typedef __attribute__((sysv_abi)) void (*tboot_entry_function)(uint32_t magic, tboot_info_t* info);

#endif //TOMATBOOT_UEFI_TBOOT_H
