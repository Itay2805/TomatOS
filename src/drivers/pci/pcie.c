#include "pcie.h"
#include "pci.h"

#include <drivers/acpi/mcfg.h>
#include <memory/vmm.h>
#include <buf.h>

bool pcie_supported() {
    return mcfg != NULL;
}

static mcfg_entry_t* current;

/**
 * Set the mmio base for the device
 */
static void set_mmio_base(pcidev_pcie_t* dev) {
    dev->mmio_base = PHYSICAL_ADDRESS(current->base + (((dev->bus - current->start_pci_bus) << 20) | (dev->device << 15) | dev->function << 12));
    if(!vmm_is_mapped(kernel_address_space, (uintptr_t) dev->mmio_base)) {
        vmm_map(kernel_address_space, dev->mmio_base, dev->mmio_base - PHYSICAL_BASE, PAGE_ATTR_WRITE);
    }
}

/**************************************************************
 * PCIe read and write functions
 **************************************************************/

// TODO: Have all of the pci read/write implemented by driver, since this can make it much faster

uint64_t pcie_config_read_64(pcidev_t* dev, uint16_t offset) {
    return *(uint64_t*)&dev->pcie.mmio_base[offset];
}

uint32_t pcie_config_read_32(pcidev_t* dev, uint16_t offset) {
    return *(uint32_t*)&dev->pcie.mmio_base[offset];
}

uint16_t pcie_config_read_16(pcidev_t* dev, uint16_t offset) {
    return *(uint16_t*)&dev->pcie.mmio_base[offset];
}

uint8_t pcie_config_read_8(pcidev_t* dev, uint16_t offset) {
    return (uint8_t)dev->pcie.mmio_base[offset];
}

/**************************************************************
 * Iterating the devices
 **************************************************************/

static void check_bus(pcidev_pcie_t* dev);

/**
 * Will check the functions
 */
static void check_function(pcidev_pcie_t* dev) {
    // set the pci device with this pci device
    pcidev_t pcidev = {};
    pcidev.pcie = *dev;

    pcidev.vendor_id = pci_config_read_16(&pcidev, PCI_VENDOR_ID);
    pcidev.device_id = pci_config_read_16(&pcidev, PCI_DEVICE_ID);
    pcidev.class = pci_config_read_8(&pcidev, PCI_CLASS_CODE);
    pcidev.subclass = pci_config_read_8(&pcidev, PCI_SUBCLASS);
    pcidev.prog_if = pci_config_read_8(&pcidev, PCI_PROG_IF);

    log_info("\t0x%016p -> %s", dev->mmio_base, pci_get_name(&pcidev));
    buf_push(pcidevs, pcidev);
}

/**
 * Will check the device for all the functions
 */
static void check_device(pcidev_pcie_t* dev) {
    dev->function = 0;
    set_mmio_base(dev);

    // check the device even exists
    uint16_t vendor_id = pci_config_read_16((pcidev_t*)dev, PCI_VENDOR_ID);
    if(vendor_id == 0xFFFF) return;

    // check the device with the first function
    check_function(dev);

    // check if has multiple functions
    uint8_t header_type = pci_config_read_8((pcidev_t*)dev, PCI_HEADER_TYPE);
    if((header_type & 0x80) != 0) {
        for(uint8_t function = 1; function < 8; function++) {
            dev->function = function;
            set_mmio_base(dev);

            // check the function exists
            vendor_id = pci_config_read_16((pcidev_t*)dev, PCI_VENDOR_ID);
            if(vendor_id == 0xFFFF) continue;

            // check this function
            check_function(dev);
        }
    }
}

/**
 * Will check the bus for all devices and functions
 * @param bus
 */
void check_bus(pcidev_pcie_t* dev) {
    for(uint8_t device = 0; device < 32; device++) {
        dev->device = device;
        check_device(dev);
    }
}

/**
 * Will check the segment
 */
static void check_segment(mcfg_entry_t* entry) {
    current = entry;
    for(uint8_t bus = entry->start_pci_bus; bus < entry->end_pci_bus; bus++) {
        pcidev_pcie_t dev = {0};
        dev.bus = bus;
        check_bus(&dev);
    }
}

error_t pcie_init() {
    log_info("iterating PCIe devices");
    for(int i = 0; i < ((mcfg->header.length - sizeof(mcfg_t)) / sizeof(mcfg_entry_t)); i++) {
        check_segment(&mcfg->entries[i]);
    }

    return NO_ERROR;
}
