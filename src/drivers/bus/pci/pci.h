#ifndef TOMATKERNEL_PCI_H
#define TOMATKERNEL_PCI_H

#include <interrupts/apic/lapic.h>
#include "pci_spec.h"

typedef struct pci_bar {
    bool port;
    uintptr_t base;
    size_t size;
} pci_bar_t;

typedef struct pci_sig {
    uint16_t vendor_id;
    uint16_t device_id;
} pci_sig_t;

typedef struct pci_device {
    // the port
    uint16_t segment;
    uint8_t bus;
    uint8_t slot;
    uint8_t function;

    // the parent of this device
    // null for the root bridge
    struct pci_device* parent;

    // the mmio
    union {
        pci_common_header_t* header;
        pci_bridge_header_t* bridge_header;
        pci_device_header_t* device_header;
        uintptr_t mmio_base;
    };

    // the bars, max of 6, for some devices may be less
    pci_bar_t bars[6];

    // the native irq, -1 for none
    int irq;

    // msi capability, null if does not exists
    msi_cap_t* msi;

    // link
    list_entry_t link;
} pci_device_t;

/**
 * Array of pci devices
 */
extern list_entry_t pci_devices;

/**
 * Will iterate all pci devices and initialize them
 */
void pci_init();

/**
 * Will get the configuration space base address for the given pci device
 *
 * Will also map it if needed
 *
 * @param seg       [IN] The segment
 * @param bus       [IN] The bus
 * @param slot      [IN] The slot
 * @param function  [IN] The function
 */
void* pci_get_config_space(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t function);

/**
 * Will get the name of the device depending on the class, subclass and prog_if
 *
 * @param class     [IN] The class
 * @param subclass  [IN] The subclass
 * @param prog_if   [IN] The program interface
 */
const char* pci_get_name(uint16_t class, uint16_t subclass, uint8_t prog_if);

/**
 * Will setup MSI for the given device
 *
 * The msi will fire in the given IRQ
 *
 * @param device    [IN] The PCI device to set the MSI to
 * @param vector    [IN] The vector to set the MSI to
 */
void pci_msi_setup(pci_device_t* device, uint8_t vector);

#endif //TOMATKERNEL_PCI_H
