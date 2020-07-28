#ifndef __TOMATOS_KERNEL_SYS_PCI_PCI_H__
#define __TOMATOS_KERNEL_SYS_PCI_PCI_H__

#include <lai/core.h>

#include <util/except.h>
#include <mem/vmm.h>
#include <stdint.h>

#include "pci_spec.h"

typedef struct pci_address {
    uint16_t seg;
    uint8_t bus;
    uint8_t slot;
    uint8_t func;
} pci_address_t;

typedef struct pci_bar {
    uint8_t present : 1;
    uint8_t mmio : 1;
    union {
        directptr_t addr;
        uint16_t port;
    };
    size_t length;
} pci_bar_t;

typedef struct pci_dev {
    /**
     * The parent of the device
     */
    struct pci_dev* parent;

    /**
     * common id stuff
     */
    uint16_t device_id;
    uint16_t vendor_id;
    uint8_t class;
    uint8_t subclass;
    uint8_t progif;

    /**
     * The address of the pci device
     */
    pci_address_t address;

    /**
     * The base addresses of the device
     * only 2 for bridges
     */
    pci_bar_t bars[6];

    /**
     * The irq of the pci device
     * already handled correctly
     */
    int irq;

    /**
     * The pci config space
     */
    volatile pci_common_header_t* header;
    union {
        volatile pci_bridge_header_t* bridge;
        volatile pci_device_header_t* device;
    };

    /**
     * The child devices of the device
     * only applies to bridges
     */
    struct pci_dev** childs;

    /**
     * The acpi node of the device
     */
    lai_nsnode_t* acpi_node;
} pci_dev_t;

/**
 * The root devices of the pci namespace
 */
extern pci_dev_t** g_pci_roots;

typedef struct pci_entry {
    pci_address_t key;
    pci_dev_t* value;
} pci_entry_t;

/**
 * Lookup table between pci address and
 * the device
 */
extern pci_entry_t* g_pci_map;

/**
 * First pci initialization, no iteration yet
 */
err_t early_init_pci();

/**
 * Take the class of a pci device and turn it into a human
 * readable string
 */
const char* get_pci_name(uint8_t class, uint8_t subclass, uint8_t progif);

/**
 * Will iterate the pci root, adding all the children
 * and parents properly
 *
 * @remark
 * This will also add the root to the root list
 *
 * @param root  [IN] The root
 */
err_t iterate_pci_root(pci_dev_t* root);

/**
 * Will get the mmio regions for the given pci address
 */
err_t get_config_for_pci(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, void** config);

#endif //__TOMATOS_KERNEL_SYS_PCI_PCI_H__
