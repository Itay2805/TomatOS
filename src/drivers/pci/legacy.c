#include <drivers/portio.h>
#include <buf.h>
#include "legacy.h"
#include "pci.h"

#define PCI_REG_ADDRESS     0xcf8
#define PCI_REG_DATA        0xcfc

bool pci_legacy_supported() {
    outl(PCI_REG_ADDRESS, 0x80000000);
    return inl(PCI_REG_ADDRESS) == 0x80000000;
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

static void check_bus(pcidev_legacy_t* dev);

static void check_function(pcidev_legacy_t* dev) {
    // set the pci device with this pci device
    pcidev_t pcidev = {};
    pcidev.legacy = *dev;

    pcidev.vendor_id = pci_config_read_16(&pcidev, PCI_VENDOR_ID);
    pcidev.device_id = pci_config_read_16(&pcidev, PCI_DEVICE_ID);
    pcidev.class = pci_config_read_8(&pcidev, PCI_CLASS_CODE);
    pcidev.subclass = pci_config_read_8(&pcidev, PCI_SUBCLASS);
    pcidev.prog_if = pci_config_read_8(&pcidev, PCI_PROG_IF);

    log_info("\t%d:%d:%d -> %s", dev->bus, dev->slot, dev->function, pci_get_name(&pcidev));
    buf_push(pcidevs, pcidev);
}

static void check_slot(pcidev_legacy_t* dev) {

    // make sure we even have this slot
    uint16_t vendor_id = pci_config_read_16((pcidev_t*)dev, PCI_VENDOR_ID);
    if(vendor_id == 0xFFFF) return;

    // check the device with the first function
    check_function(dev);

    // check if has multiple functions
    uint8_t header_type = pci_config_read_8((pcidev_t*)dev, PCI_HEADER_TYPE);
    if((header_type & 0x80) != 0) {
        for(uint8_t function = 1; function < 8; function++) {
            dev->function = function;

            // check the function exists
            vendor_id = pci_config_read_16((pcidev_t*)dev, PCI_VENDOR_ID);
            if(vendor_id == 0xFFFF) continue;

            // check this function
            check_function(dev);
        }
    }
}

void check_bus(pcidev_legacy_t* dev) {
    for(uint8_t slot = 0; slot < 32; slot++) {
        dev->slot = slot;
        check_slot(dev);
    }
}

static void check_all_buses() {
    pcidev_legacy_t dev = {};
    dev.bus = 0;
    dev.slot = 0;
    dev.function = 0;

    if((pci_config_read_8((pcidev_t *)&dev, PCI_HEADER_TYPE) & 0x80) == 0) {
        check_bus(&dev);
    }else {
        for(uint8_t function = 0; function < 8; function++) {
            if(pci_config_read_16((pcidev_t *) &dev, PCI_VENDOR_ID) == 0xFFFF) continue;
            dev.bus = function;
            check_bus(&dev);
        }
    }
}

error_t pci_legacy_init() {
    log_info("iterating Legacy PCI devices");
    check_all_buses();
    return NO_ERROR;
}
