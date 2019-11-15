#ifndef TOMATKERNEL_PCI_SPEC_H
#define TOMATKERNEL_PCI_SPEC_H

#include <interrupts/apic/lapic.h>
#include <libc/stddef.h>

typedef struct pci_reg_command {
    uint16_t _reserved0  : 2;                           // 0-1
    uint16_t bus_master_enable : 1;                     // 2
    uint16_t special_cycle_enable : 1;                  // 3
    uint16_t memory_write_and_invalidate : 1;           // 4
    uint16_t vga_palette_snoop : 1;                     // 5
    uint16_t parity_error_response : 1;                 // 6
    uint16_t idsel_stepping_wait_cycle_control : 1;     // 7
    uint16_t serr_enable : 1;                           // 8
    uint16_t fast_back_to_back_transaction_enable : 1;  // 9
    uint16_t interrupt_disable : 1;                     // 10
} __attribute__((packed)) pci_reg_command_t;
_Static_assert(sizeof(pci_reg_command_t) == sizeof(uint16_t), "");

typedef struct pci_reg_status {
    uint16_t _reserved0 : 3;                            // 0-2
    uint16_t interrupt : 1;                             // 3
    uint16_t caps_list : 1;                             // 4
    uint16_t enable_66mhz : 1;                          // 5
    uint16_t _reserved1 : 1;                            // 6
    uint16_t fast_back_to_back_transaction_capable : 1; // 7
    uint16_t master_data_parity_error : 1;              // 8
    uint16_t devsel_timing : 2;                         // 9-10
    uint16_t signaled_target_abort : 1;                 // 11
    uint16_t received_target_abort : 1;                 // 12
    uint16_t received_master_abort : 1;                 // 13
    uint16_t signaled_system_error : 1;                 // 14
    uint16_t detected_parity_error : 1;                 // 15
} __attribute__((packed)) pci_reg_status_t;
_Static_assert(sizeof(pci_reg_status_t) == sizeof(uint16_t), "");

#pragma pack(4)
typedef struct pci_common_header {
    volatile uint16_t vendor_id;
    volatile uint16_t device_id;
    pci_reg_command_t command;
    volatile pci_reg_status_t status;
    uint8_t revision_id;
    uint8_t program_interface;
    uint8_t subclass_code;
    uint8_t class_code;
    uint8_t cache_line_size;
    uint8_t master_latency_timer;
    uint8_t header_type : 7;
    uint8_t multifunction : 1;
    uint8_t _reserved0[37];
    uint8_t caps_pointer;
    uint8_t _reserved1[7];
    uint8_t interrupt_line;
    uint8_t interrupt_pin;
    uint8_t _reserved[2];
} pci_common_header_t;
_Static_assert(sizeof(pci_common_header_t) == 0x40, "");
#pragma pack()

#pragma pack(4)
typedef struct pci_device_header {
    uint16_t vendor_id;
    uint16_t device_id;
    pci_reg_command_t command;
    pci_reg_status_t status;
    uint8_t revision_id;
    uint8_t program_interface;
    uint8_t subclass_code;
    uint8_t class_code;
    uint8_t cache_line_size;
    uint8_t master_latency_timer;
    uint8_t header_type : 7;
    uint8_t multifunction : 1;
    uint8_t bist;
    uint32_t bar0;
    uint32_t bar1;
    uint32_t bar2;
    uint32_t bar3;
    uint32_t bar4;
    uint32_t bar5;
    uint32_t cardbus_cis_pointer;
    uint16_t subsystem_vendor_id;
    uint16_t subsystem_id;
    uint32_t expansion_rom;
    uint8_t caps_pointer;
    uint8_t _reserved1[7];
    uint8_t interrupt_line;
    uint8_t interrupt_pin;
    uint8_t min_grant;
    uint8_t max_latency;
} pci_device_header_t;
_Static_assert(sizeof(pci_device_header_t) == 0x40, "");
#pragma pack()

#pragma pack(4)
typedef struct pci_bridge_header {
    uint16_t vendor_id;
    uint16_t device_id;
    pci_reg_command_t command;
    pci_reg_status_t status;
    uint8_t revision_id;
    uint8_t program_interface;
    uint8_t subclass_code;
    uint8_t class_code;
    uint8_t cache_line_size;
    uint8_t master_latency_timer;
    uint8_t header_type : 7;
    uint8_t multifunction : 1;
    uint8_t bist;
    uint32_t bar0;
    uint32_t bar1;
    uint8_t primary_bus_number;
    uint8_t secondary_bus_number;
    uint8_t subordinate_bus_number;
    uint8_t secondary_latency_timer;
    uint8_t io_base;
    uint8_t io_limit;
    pci_reg_status_t secondary_status;
    uint16_t memory_base;
    uint16_t memory_limit;
    uint16_t prefetchable_memory_base;
    uint16_t prefetchable_memory_limit;
    uint32_t prefetchable_memory_base_upper;
    uint32_t prefetchable_memory_limit_upper;
    uint16_t io_base_upper;
    uint16_t io_limit_upper;
    uint8_t caps_pointer;
    uint8_t _reserved1[3];
    uint32_t expansion_rom_address;
    uint8_t interrupt_line;
    uint8_t interrupt_pin;
    uint16_t bridge_control;
} pci_bridge_header_t;
_Static_assert(sizeof(pci_bridge_header_t) == 0x40, "");
#pragma pack()

typedef struct msi_cap {
    uint8_t cap_id;
    uint8_t next;
    struct {
        uint16_t msi_enable : 1;
        uint16_t multiple_message_capable : 3;
        uint16_t multiple_message_enable : 3;
        uint16_t bit64_capable : 1;
        uint16_t _reserved0 : 8;
    };
    union {
        struct {
            lapic_msi_address_t address_low;
            uint32_t address_high;
            lapic_msi_data_t data;
        } bit64;
        struct {
            lapic_msi_address_t address;
            lapic_msi_data_t data;
        } bit32;
    };
} __attribute__((packed)) msi_cap_t;

// Bridge device ids
#define PCI_CLASS_BRIDGE_DEVICE         0x06
#define PCI_SUBCLASS_HOST_BRIDGE        0x00
#define PCI_SUBCLASS_PCI_TO_PCI_BRIDGE  0x04

#endif //TOMATKERNEL_PCI_SPEC_H
