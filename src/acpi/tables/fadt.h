#ifndef TOMATKERNEL_FADT_H
#define TOMATKERNEL_FADT_H

#include <error.h>
#include <acpi/acpi.h>
#include "rsdt.h"

typedef struct fadt {
    sdt_hdr_t header;
    uint32_t firmware_control;
    uint32_t dsdt;
    uint8_t _reserved0;
    uint8_t profile;
    uint16_t sci_irq;
    uint32_t smi_command_port;
    uint8_t acpi_enable;
    uint8_t acpi_disable;
    uint8_t s4bios_req;
    uint8_t pstate_control;
    uint32_t pm1a_event_block;
    uint32_t pm1b_event_block;
    uint32_t pm1a_control_block;
    uint32_t pm1b_control_block;
    uint32_t pm2_control_block;
    uint32_t pm_timer_block;
    uint32_t gpe0_block;
    uint32_t gpe1_block;
    uint8_t pm1_event_length;
    uint8_t pm1_control_length;
    uint8_t pm2_control_length;
    uint8_t pm_timer_length;
    uint8_t gpe0_length;
    uint8_t gpe1_length;
    uint8_t gpe1_base;
    uint8_t cstate_control;
    uint16_t worst_c2_latency;
    uint16_t worst_c3_latency;
    uint16_t flush_size;
    uint16_t flush_stride;
    uint8_t duty_offset;
    uint8_t duty_width;
    uint8_t day_alarm;
    uint8_t month_alarm;
    uint8_t century;

    struct {
        uint16_t boot_flags;
        uint8_t _reserved2;
        uint32_t flags;

        acpi_generic_address_t reset_register;
        uint8_t reset_command;
        uint8_t _reserved3[3];

        uint64_t x_firmware_control;
        uint64_t x_dsdt;

        acpi_generic_address_t x_pm1a_event_block;
        acpi_generic_address_t x_pm1b_event_block;
        acpi_generic_address_t x_pm1a_control_block;
        acpi_generic_address_t x_pm1b_control_block;
        acpi_generic_address_t x_pm2_control_block;
        acpi_generic_address_t x_pm_timer_block;
        acpi_generic_address_t x_gpe0_block;
        acpi_generic_address_t x_gpe1_block;
    } __attribute__((packed)) acpi2;
} __attribute__((packed)) fadt_t;

extern fadt_t* fadt;
extern sdt_hdr_t* dsdt;

error_t fadt_init();

#endif //TOMATKERNEL_FADT_H
