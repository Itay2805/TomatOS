#ifndef TOMATKERNEL_DRIVERS_PCI_H
#define TOMATKERNEL_DRIVERS_PCI_H

#include <common/except.h>

// TODO: PCI-to-PCI bridge/PCI-to-BUSCARD bridge

typedef struct pci_device {
    uint16_t bus;
    uint16_t slot;
    uint16_t function;
    uint16_t vendor;
    uint16_t device;
    uint8_t class;
    uint8_t subclass;
    uint8_t programming_interface;
    uint8_t revision_id;
} pci_device_t;

extern pci_device_t* pci_devices;

error_t pci_init();

#define PCI_REG_BAR0    0x10
#define PCI_REG_BAR1    0x14
#define PCI_REG_BAR2    0x18
#define PCI_REG_BAR3    0x1C
#define PCI_REG_BAR4    0x20
#define PCI_REG_BAR5    0x24
#define PCI_REG_CAP     0x34

uint16_t pci_read_uint16(pci_device_t* device, uint16_t reg);
void pci_write_uint16(pci_device_t* device, uint16_t reg, uint16_t data);

uint32_t pci_read_uint32(pci_device_t* device, uint16_t reg);
void pci_write_uint32(pci_device_t* device, uint16_t reg, uint32_t data);

#endif