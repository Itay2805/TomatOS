#ifndef __TOMATOS_KERNEL_SYS_PCI_PCI_SPEC_H__
#define __TOMATOS_KERNEL_SYS_PCI_PCI_SPEC_H__

#include <util/defs.h>
#include <stdint.h>
#include <acpispec/tables.h>

typedef struct pci_command {
    uint16_t io_space : 1;
    uint16_t memory_space : 1;
    uint16_t bus_master : 1;
    uint16_t special_cycle : 1;
    uint16_t memory_write_and_invalidate : 1;
    uint16_t vga_palette_snoop : 1;
    uint16_t parity_error_respond : 1;
    uint16_t stepping_control : 1;
    uint16_t serr : 1;
    uint16_t fast_back_to_back : 1;
} PACKED pci_command_t;

typedef struct pci_status {
    uint16_t _reserved1 : 4;
    uint16_t capability : 1;
    uint16_t capable_66mz : 1;
    uint16_t _reserved2 : 1;
    uint16_t fast_back_to_back_capable : 1;
    uint16_t master_data_parity_error : 1;
} PACKED pci_status_t;

typedef struct pci_common_header {
    uint16_t vendor_id;
    uint16_t device_id;
    pci_command_t command;
    pci_status_t status;
    uint8_t revision_id;
    uint8_t progif;
    uint8_t subclass;
    uint8_t class;
    uint8_t cache_line_size;
    uint8_t latency_timer;
    uint8_t header_type;
    uint8_t bist;
} PACKED pci_common_header_t;

typedef struct pci_device_header {
    uint32_t bar[6];
    uint32_t cisptr;
    uint16_t subsystem_vendor_id;
    uint16_t subsystem_id;
    uint32_t expansion_rom;
    uint8_t capability_ptr;
    uint8_t _reserved[3];
    uint32_t _reserved2;
    uint8_t interrupt_line;
    uint8_t interrupt_pin;
    uint8_t min_gnt;
    uint8_t max_lat;
} PACKED pci_device_header_t;

typedef struct pci_bridge_command {
    uint16_t parity_error_response : 1;
    uint16_t serr : 1;
    uint16_t isa : 1;
    uint16_t vga : 1;
    uint16_t vga_16 : 1;
    uint16_t master_abort : 1;
    uint16_t reset_secondary_bus : 1;
    uint16_t fast_back_to_back : 1;
    uint16_t primary_disacrd_timer : 1;
    uint16_t secondary_discard_timer : 1;
    uint16_t timer_status : 1;
    uint16_t discard_timer_serr : 1;
} PACKED pci_bridge_command_t;

typedef struct pci_bridge_header {
    uint32_t bar[2];
    uint8_t primary_bus;
    uint8_t secondary_bus;
    uint8_t subordinate_bus;
    uint8_t secondary_latency_timer;
    uint8_t io_base;
    uint8_t io_limit;
    uint16_t secondary_status;
    uint16_t memory_base;
    uint16_t memory_limit;
    uint16_t prefetchable_memory_base;
    uint16_t prefetchable_memory_limit;
    uint32_t prefetchable_base_upper32;
    uint32_t prefetchable_limit_upper32;
    uint16_t io_base_upper16;
    uint16_t io_limit_upper16;
    uint8_t _reserved[3];
    uint32_t expansion_bar;
    uint8_t interrupt_line;
    uint8_t interrupt_pin;
    uint16_t bridge_control;
} PACKED pci_bridge_header_t;

typedef struct acpi_mcfg_entry {
    uint64_t base_address;
    uint16_t pci_segment;
    uint8_t start_bus;
    uint8_t end_bus;
    uint32_t _reserved;
} PACKED acpi_mcfg_entry_t;

typedef struct acpi_mcfg {
    acpi_header_t header;
    uint64_t _reserved;
    acpi_mcfg_entry_t entries[0];
} PACKED acpi_mcfg_t;

#endif //__TOMATOS_KERNEL_SYS_PCI_PCI_SPEC_H__
