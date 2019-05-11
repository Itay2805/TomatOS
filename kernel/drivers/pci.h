#ifndef TOMATKERNEL_DRIVERS_PCI_H
#define TOMATKERNEL_DRIVERS_PCI_H

#include <common/except.h>

// TODO: PCI-to-PCI bridge

typedef struct pci_device {
    uint16_t bus;
    uint16_t slot;
    uint16_t function;
    uint16_t vendor;
    uint16_t device;
    uint16_t class;
    uint16_t subclass;
    uint16_t prog_if;
    uint16_t revision_id;
} pci_device_t;

extern pci_device_t* pci_devices;

error_t pci_init();

uint16_t pci_read(uint16_t bus, uint16_t slot, uint16_t func, uint16_t reg);

void pci_write(uint16_t bus, uint16_t slot, uint16_t func, uint16_t reg, uint16_t data);

#endif