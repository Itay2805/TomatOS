#ifndef TOMATKERNEL_PCI_H
#define TOMATKERNEL_PCI_H

#include <libc/stdint.h>
#include <common/error.h>

typedef union pciport {
    struct {
        uint16_t segment;
        uint8_t bus;
        uint8_t device;
        uint8_t function;
    };
    uint64_t raw;
} pciport_t;


#define PCIPORT(_segment, _bus, _device, _function) (((pciport_t){ .segment = _segment, .bus = _bus, .device = _device, .function = _function }).raw << 24)

typedef struct pci_bar {
    uint8_t type;
    uintptr_t base;
    size_t len;
} pci_bar_t;

typedef struct pci_cap {
    uint8_t cap_id;
    char* data;
} pci_cap_t;

typedef struct pci_dev {
    struct pci_dev* parent;

    // the address
    uint16_t segment;
    uint8_t bus;
    uint8_t device;
    uint8_t function;

    // id
    uint16_t vendor_id;
    uint16_t device_id;
    uint16_t revision_id;

    // class
    uint8_t class;
    uint8_t subclass;
    uint8_t prog_if;

    // other extracted info
    pci_bar_t* bars;
    pci_cap_t* caps;
    int irq;

    // the mmio base address
    char* mmio;
} pci_dev_t;

typedef struct pci_sig {
    uint16_t vendor_id;
    uint16_t device_id;
} pci_sig_t;

extern pci_dev_t** pci_devices;

// read the mmio space
uint32_t pci_read_32(pci_dev_t* dev, uint16_t offset);
uint16_t pci_read_16(pci_dev_t* dev, uint16_t offset);
uint8_t pci_read_8(pci_dev_t* dev, uint16_t offset);

void pci_write_32(pci_dev_t* dev, uint16_t offset, uint32_t data);
void pci_write_16(pci_dev_t* dev, uint16_t offset, uint16_t data);
void pci_write_8(pci_dev_t* dev, uint16_t offset, uint8_t data);

error_t pci_get_and_map_mmio(uint16_t segment, uint8_t bus, uint8_t device, uint8_t function, char** mmio);

/**
 * This will:
 * - Enable bus master
 * - Enable interrupts
 */
error_t pci_setup_device(pci_dev_t* dev);

error_t pci_init();

#endif //TOMATKERNEL_PCI_H
