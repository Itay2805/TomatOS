#ifndef TOMATOS_PCI_MSI_H
#define TOMATOS_PCI_MSI_H

#include <stdint.h>

typedef union msi_address {
    struct {
        uint32_t _reserved0 : 2;
        uint32_t destination_mode : 1;
        uint32_t redirection_hint : 1;
        uint32_t _reserved1 : 8;
        uint32_t destination_id : 8;
        // must be 0xFEE
        uint32_t base_address : 12;
    };
    uint32_t raw;
} __attribute__((packed)) msi_address_t;

typedef union msi_data {
    struct {
        uint32_t vector : 8;
        uint32_t delivery_mode : 3;
        uint32_t _reserved0 : 3;
        uint32_t level : 1;
        uint32_t trigger_mode : 1;
        uint32_t _reserved1 : 16;
    };
} __attribute__((packed)) msi_data_t;

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
            msi_address_t address_low;
            uint32_t address_high;
            msi_data_t data;
        } bit64;
        struct {
            msi_address_t address;
            msi_data_t data;
        } bit32;
    };
} msi_cap_t;

#endif //TOMATOS_PCI_MSI_H
