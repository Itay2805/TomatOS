#ifndef TOMATKERNEL_RSDP_H
#define TOMATKERNEL_RSDP_H

#include "tables.h"

typedef struct acpi_rsdp {
    char signature[8];
    uint8_t checksum;
    char oem[6];
    uint8_t revision;
    uint32_t rsdt;
} __attribute__((packed)) acpi_rsdp1_t;

typedef struct acpi_rsdp2 {
    char signature[8];
    uint8_t checksum;
    char oem[6];
    uint8_t revision;
    uint32_t rsdt;
    uint32_t length;
    uint64_t xsdt;
    uint8_t extended_checksum;
} __attribute__((packed)) acpi_rsdp2_t;

extern acpi_rsdp2_t* acpi_rsdp2;
extern acpi_rsdp1_t* acpi_rsdp;

/**
 * Will search for the rsdp
 */
void rsdp_init(tboot_info_t* info);

#endif //TOMATKERNEL_RSDP_H
