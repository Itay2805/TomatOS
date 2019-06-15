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

static void set_address(pcidev_t* dev, uint16_t offset) {
    uint32_t addr = (dev->bus << 16) | (dev->device << 11) | (dev->function << 8) | (offset & 0xff) | 0x80000000;
    outl(PCI_REG_ADDRESS, addr);
}

uint64_t pci_legacy_config_read_64(pcidev_t* dev, uint16_t offset) {
    return (((uint64_t)pci_legacy_config_read_32(dev, (uint16_t) (offset + 4)) << 32) | pci_legacy_config_read_32(dev, offset));
}

uint32_t pci_legacy_config_read_32(pcidev_t* dev, uint16_t offset) {
    set_address(dev, offset);
    return inl(PCI_REG_DATA);
}

uint16_t pci_legacy_config_read_16(pcidev_t* dev, uint16_t offset) {
    set_address(dev, offset);
    return inw(PCI_REG_DATA);
}

uint8_t pci_legacy_config_read_8(pcidev_t* dev, uint16_t offset) {
    set_address(dev, offset);
    return inb(PCI_REG_DATA);
}

void pci_legacy_config_write_64(pcidev_t* dev, uint16_t offset, uint64_t value) {
    pci_legacy_config_write_32(dev, offset, (uint32_t) (value & 0xFFFFFFFF));
    pci_legacy_config_write_32(dev, (uint16_t) (offset + 4), (uint32_t) (value >> 32 & 0xFFFFFFFF));
}

void pci_legacy_config_write_32(pcidev_t* dev, uint16_t offset, uint32_t value) {
    set_address(dev, offset);
    outl(PCI_REG_DATA, value);
}

void pci_legacy_config_write_16(pcidev_t* dev, uint16_t offset, uint16_t value) {
    set_address(dev, offset);
    outw(PCI_REG_DATA, value);
}

void pci_legacy_config_write_8(pcidev_t* dev, uint16_t offset, uint8_t value) {
    set_address(dev, offset);
    outb(PCI_REG_DATA, value);
}

/**************************************************************
 * Iterating the devices
 **************************************************************/

static void check_bus(pcidev_t* dev);

static void check_function(pcidev_t* dev) {
    // set the pci device with this pci device
    dev->vendor_id = pci_config_read_16(dev, PCI_VENDOR_ID);
    dev->device_id = pci_config_read_16(dev, PCI_DEVICE_ID);
    dev->class = pci_config_read_8(dev, PCI_CLASS_CODE);
    dev->subclass = pci_config_read_8(dev, PCI_SUBCLASS);
    dev->prog_if = pci_config_read_8(dev, PCI_PROG_IF);

    log_info("\t%x.%x.%x -> %s", dev->bus, dev->device, dev->function, pci_get_name(dev));
    buf_push(pcidevs, *dev);
}

static void check_slot(pcidev_t* dev) {

    // make sure we even have this slot
    uint16_t vendor_id = pci_config_read_16(dev, PCI_VENDOR_ID);
    if(vendor_id == 0xFFFF) return;

    // check the device with the first function
    check_function(dev);

    // check if has multiple functions
    uint8_t header_type = pci_config_read_8(dev, PCI_HEADER_TYPE);
    if((header_type & 0x80) != 0) {
        for(uint8_t function = 1; function < 8; function++) {
            dev->function = function;

            // check the function exists
            vendor_id = pci_config_read_16(dev, PCI_VENDOR_ID);
            if(vendor_id == 0xFFFF) continue;

            // check this function
            check_function(dev);
        }
    }
}

void check_bus(pcidev_t* dev) {
    for(uint8_t device = 0; device < 32; device++) {
        dev->device = device;
        check_slot(dev);
    }
}

static void check_all_buses() {
    pcidev_t dev = {};
    dev.bus = 0;
    dev.device = 0;
    dev.function = 0;

    if((pci_config_read_8(&dev, PCI_HEADER_TYPE) & 0x80) == 0) {
        check_bus(&dev);
    }else {
        for(uint8_t function = 0; function < 8; function++) {
            if(pci_config_read_16(&dev, PCI_VENDOR_ID) == 0xFFFF) continue;
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
