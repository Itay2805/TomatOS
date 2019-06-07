#include <drivers/portio.h>
#include "legacy.h"
#include "pci.h"

#define PCI_REG_ADDRESS     0xcf8
#define PCI_REG_DATA        0xcfc

bool pci_legacy_supported() {
    outl(PCI_REG_ADDRESS, 0x80000000);
    return inl(PCI_REG_ADDRESS) != 0x80000000;
}

/**************************************************************
 * Legacy PCI read and write functions
 **************************************************************/

static void set_address(struct pcidev* dev, uint16_t offset) {
    uint32_t addr = (dev->legacy.bus << 16) | (dev->legacy.slot << 11) | (dev->legacy.function << 8) | (offset & 0xff) | 0x80000000;
    outl(PCI_REG_ADDRESS, addr);
}

uint64_t pci_legacy_config_read_64(struct pcidev* dev, uint16_t offset) {
    return (((uint64_t)pci_legacy_config_read_32(dev, (uint16_t) (offset + 4)) << 32) | pci_legacy_config_read_32(dev, offset));
}

uint32_t pci_legacy_config_read_32(struct pcidev* dev, uint16_t offset) {
    set_address(dev, offset);
    return inl(PCI_REG_DATA);
}

uint16_t pci_legacy_config_read_16(struct pcidev* dev, uint16_t offset) {
    set_address(dev, offset);
    return inw(PCI_REG_DATA);
}

uint8_t pci_legacy_config_read_8(struct pcidev* dev, uint16_t offset) {
    set_address(dev, offset);
    return inb(PCI_REG_DATA);
}

/**************************************************************
 * Iterating the devices
 **************************************************************/

error_t pci_legacy_init() {
    error_t err = NO_ERROR;

    // TODO:
    CHECK_FAIL();

cleanup:
    return err;
}
