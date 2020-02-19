#ifndef __ACPI_TABLES_RSDP_H__
#define __ACPI_TABLES_RSDP_H__

#include <stdint.h>
#include <tboot.h>

typedef struct acpi_rsdp_t {
    char signature[8];
    uint8_t checksum;
    char oem[6];
    uint8_t revision;
    uint32_t rsdt;
}__attribute__((packed)) acpi_rsdp_t;

typedef struct acpi_xsdp_t {
    char signature[8];
    uint8_t checksum;
    char oem[6];
    uint8_t revision;
    uint32_t rsdt;
    uint32_t length;
    uint64_t xsdt;
    uint8_t extended_checksum;
}__attribute__((packed)) acpi_xsdp_t;

extern acpi_rsdp_t* acpi_rsdp;
extern acpi_xsdp_t* acpi_xsdp;

void rsdp_init(tboot_info_t* info);

#endif //__ACPI_TABLES_RSDP_H__
