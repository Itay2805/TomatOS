#include <libc/stdlib.h>
#include <acpi/tables/mcfg.h>
#include <memory/vmm.h>
#include <stb/stb_ds.h>
#include <common/common.h>
#include <lai/helpers/pci.h>
#include <lai/helpers/resource.h>
#include "pci.h"
#include "pci_common_spec.h"
#include "pci_bridge_spec.h"
#include "pci_device_spec.h"
#include "pci_ids.h"

// used
pci_dev_t** pci_devices;

// used to set the current parent
static pci_dev_t** bus_stack;

// quick switch to get the name of a pci device class/subclass
const char* pci_get_name(pci_dev_t* dev) {
    switch(dev->class) {
        case 0x00:
            switch(dev->subclass) {
                case 0x00: return "Non-VGA-Compatible device";
                case 0x01: return "VGA-Compatible device";
                default: return "Unclassified";
            }
        case 0x01:
            switch(dev->subclass) {
                case 0x00: return "SCSI Bus Controller";
                case 0x01: return "IDE Controller";
                case 0x02: return "Floppy Disk Controller";
                case 0x03: return "IPI Bus Controller";
                case 0x04: return "RAID Controller";
                case 0x05:
                    switch(dev->prog_if) {
                        case 0x20: return "ATA Controller (Single DMA)";
                        case 0x30: return "ATA Controller (Chained DMA)";
                        default: return "ATA Controller";
                    }
                case 0x06:
                    switch(dev->prog_if) {
                        case 0x00: return "SATA (Vendor Specific)";
                        case 0x01: return "SATA (AHCI 1.0)";
                        case 0x02: return "Serial Storage Bus (SATA)";
                        default: return "Serial ATA";
                    }
                case 0x07:
                    switch(dev->prog_if) {
                        case 0x00: return "SAS";
                        case 0x01: return "Serial Storage Bus (SAS)";
                        default: return "Serial Attached SCSI";
                    }
                case 0x08:
                    switch(dev->prog_if) {
                        case 0x01: return "NVMHCI";
                        case 0x02: return "NVM Express";
                        default: return "Non-Volatile Memory Controller";
                    }
                default: return "Mass Storage Controller";
            }
        case 0x02:
            switch(dev->subclass) {
                case 0x00: return "Ethernet Controller";
                case 0x01: return "Token Ring Controller";
                case 0x02: return "Network Controller (FDDI)";
                case 0x03: return "Network Controller (ATM)";
                case 0x04: return "Network Controller (ISDN)";
                case 0x05: return "Network Controller (WorldFip)";
                case 0x06: return "PICMG 2.14 Multi-Computing";
                case 0x07: return "Network Controller (Infiniband)";
                case 0x08: return "Network Controller (Fabric)";
                default: return "Network Controller";
            }
        case 0x03:
            switch(dev->subclass) {
                case 0x00:
                    switch(dev->prog_if) {
                        case 0x00: return "VGA Controller";
                        case 0x01: return "8514-Compatible Controller";
                        default: return "VGA-Compatible Controller";
                    }
                case 0x01: return "XGA Controller";
                case 0x02: return "3D Controller (Non VGA-Compatible)";
                default: return "Display Controller";
            }
        case 0x04: {
            switch(dev->subclass) {
                case 0x00: return "Multimedia Video Controller";
                case 0x01: return "Multimedia Audio Controller";
                case 0x02: return "Computer Telephony Device";
                case 0x03: return "Audio Device";
                default: return "Multimedia Controller";
            }
        }
        case 0x05:
            switch(dev->subclass) {
                case 0x00: return "RAM Controller";
                case 0x01: return "Flash Controller";
                default: return "Memory Controller";
            }
        case 0x06: {
            switch(dev->subclass) {
                case 0x00: return "Host bridge";
                case 0x01: return "ISA bridge";
                case 0x02: return "EISA bridge";
                case 0x04: return "PCI-to-PCI bridge";
                case 0x05: return "PCMCIA bridge";
                case 0x06: return "NuBus bridge";
                case 0x07: return "CardBus bridge";
                case 0x08: return "RACEway bridge";
                case 0x09: return "PCI-to-PCI bridge";
                case 0x0A: return "InfiniBand-to-PCI Host bridge";
                default: return "Bridge Device";
            }
        }
        case 0x07: {
            switch(dev->subclass) {
                case 0x00: {
                    switch(dev->prog_if) {
                        case 0x00: return "Serial controller (8250)";
                        case 0x01: return "Serial controller (16450)";
                        case 0x02: return "Serial controller (16550)";
                        case 0x03: return "Serial controller (16650)";
                        case 0x04: return "Serial controller (16750)";
                        case 0x05: return "Serial controller (16850)";
                        case 0x06: return "Serial controller (16950)";
                        default: return "Serial controller";
                    }
                }
                case 0x01: {
                    switch(dev->prog_if) {
                        case 0x00: return "Parallel controller (SPP)";
                        case 0x01: return "Parallel controller (BiDir)";
                        case 0x02: return "Parallel controller (ECP)";
                        case 0x03: return "Parallel controller (IEEE1284)";
                        case 0xfe: return "Parallel controller (IEEE1284 Target)";
                        default: return "Parallel controller";
                    }
                }
                case 0x02: return "Multiport serial controller";
                case 0x03: return "Modem";
                case 0x04: return "GPIB controller";
                case 0x05: return "Smard Card controller";
                default: return "Communication controller";
            }
        };
        case 0x08:
            switch(dev->subclass) {
                case 0x00:
                    switch(dev->prog_if) {
                        case 0x00: return "PIC 8259";
                        case 0x01: return "ISA PIC";
                        case 0x02: return "EISA PIC";
                        case 0x10: return "I/O APIC Interrupt Controller";
                        case 0x20: return "I/O(x) APIC Interrupt Controller";
                        default: return "PIC";
                    }
                case 0x01:
                    switch(dev->prog_if) {
                        case 0x00: return "DMA Controller (Generic 8254-Compatible)";
                        case 0x01: return "DMA Controller (ISA-Compatible)";
                        case 0x02: return "DMA Controller (EISA-Compatible)";
                        default: return "DMA Controller";
                    }
                case 0x02:
                    switch(dev->prog_if) {
                        case 0x00: return "Timer (Generic 8254-Compatible)";
                        case 0x01: return "Timer (ISA-Compatible)";
                        case 0x02: return "Timer (EISA-Compatible)";
                        case 0x03: return "Timer (HPET)";
                        default: return "Timer";
                    }
                case 0x03:
                    switch(dev->prog_if) {
                        case 0x00: return "Generic RTC";
                        case 0x01: return "RTC (ISA-Compatible)";
                        default: return "RTC";
                    }
                case 0x04: return "PCI Hot-Plug Controller";
                case 0x05: return "SD Host Controller";
                case 0x06: return "IOMMU";
                default: return "System peripheral";
            }
        case 0x09:
            switch(dev->subclass) {
                case 0x00: return "Keyboard Controller";
                case 0x01: return "Defitizer Pen";
                case 0x02: return "Mouse Controller";
                case 0x03: return "Scanner Controller";
                case 0x04: return "Gameport Controller";
                default: return "Input Device Controller";
            }
        case 0xA: return "Docking Station";
        case 0xB:
            switch(dev->subclass) {
                case 0x00: return "Processor (386)";
                case 0x01: return "Processor (486)";
                case 0x02: return "Processor (Pentium)";
                case 0x03: return "Processor (Pentium Pro)";
                case 0x10: return "Processor (Alpha)";
                case 0x20: return "Processor (PowerPC)";
                case 0x30: return "Processor (MIPS)";
                case 0x40: return "Co-Processor";
                default: return "Processor";
            }
        case 0x0C:
            switch(dev->subclass) {
                case 0x00:
                    switch(dev->prog_if) {
                        case 0x00: return "FireWire (IEEE 1394) Controller (Generic)";
                        case 0x10: return "FireWire (IEEE 1394) Controller (OHCI)";
                        default: return "FireWire (IEEE 1394) Controller";
                    }
                case 0x01: return "ACCESS bus";
                case 0x02: return "SSA";
                case 0x03:
                    switch(dev->prog_if) {
                        case 0x00: return "USB1 Controller (UHCI)";
                        case 0x10: return "USB1 Controller (OHCI)";
                        case 0x20: return "USB2 Controller (EHCI)";
                        case 0x30: return "USB3 Controller (XHCI)";
                        case 0xFE: return "USB Device";
                        default: return "USB controller";
                    }
                case 0x4: return "Fibre Channel";
                case 0x05: return "SMBus";
                case 0x06: return "Serial Bus Controller (InfiniBand)";
                case 0x07:
                    switch(dev->prog_if) {
                        case 0x00: return "IPMI Interface (SMIC)";
                        case 0x01: return "IPMI Interface (Keyboard style)";
                        case 0x02: return "IPMI Interface (Block Transfer)";
                        default: return "IPMI Interface";
                    }
                case 0x08: return "SERCOS Interface (IEC 61491)";
                case 0x09: return "Serial Bus Controller (CANbus)";
                default: return "Serial Bus Controller";
            }
        case 0x0D: {
            switch(dev->subclass) {
                case 0x00: return "iRAD Compatible Controller";
                case 0x01: return "Consumer IR Controller";
                case 0x10: return "RF Controller";
                case 0x11: return "Bluetooth Controller";
                case 0x12: return "Broadband Controller";
                case 0x20: return "Ethernet Controller (802.1a)";
                case 0x21: return "Ethernet Controller (802.1b)";
                default: return "Wireless Controller";
            }
        }
        case 0x0E:
            switch(dev->subclass) {
                case 0x00: return "Intelligent Controller (I20)";
                default: return "Intelligent Controller";
            }
        case 0x0F: return "Satellite Communication Controller";
        case 0x10: return "Encryption Controller";
        case 0x11: {
            switch(dev->subclass) {
                case 0x00: return "DPIO Modules";
                case 0x01: return "Performance Counters";
                case 0x10: return "Communication Synchronizer";
                case 0x20: return "Signal Processing Management";
                default: return "Signal Processing Controller";
            }
        }
        case 0x12: return "Processing Accelarator";
        case 0x13: return "Non-Essential Instrumentation";
        case 0x40: return "Co-Processor";
        default:
            return "Unknown";
    }
}

////////////////////////////////////////////////////////////
// Helpers for device initialization
////////////////////////////////////////////////////////////

static error_t route_device_irq(pci_dev_t* dev) {
    error_t err = NO_ERROR;
    uint8_t pin = pci_read_8(dev, PCI_REG_INTERRUPT_PIN);
    acpi_resource_t resource = {0};

    // if pin is zero then this device has no interrupts
    if(pin == 0) goto cleanup;

    // do the pin routing
    pci_dev_t* bridge = dev->parent;
    while(bridge != NULL) {
        // TODO: _PRT?
        pin = (uint8_t) ((((pin - 1) + (bridge->device % 4)) % 4) + 1);
        bridge = bridge->parent;
    }

    // do the final route
    CHECK_TRACE(
            lai_pci_route_pin(&resource, dev->segment, dev->bus, dev->device, dev->function, pin) == 0,
            "\t\tFailed to route IRQ (lai_pci_route_pin returned none-zero)");

    // got it
    dev->irq = (uint8_t) resource.base;
    log_info("\t\tIRQ #%d", dev->irq);

    cleanup:
    return err;
}

////////////////////////////////////////////////////////////
// Recursive scanning related
////////////////////////////////////////////////////////////

// forward declare
static error_t init_pci_device(uint16_t segment, uint8_t bus, uint8_t device, uint8_t function);

/**
 * Will check the bus for all devices
 *
 * @param segment   [IN] The segment
 * @param bus       [IN] The bus
 */
static error_t scan_bus(uint16_t segment, uint8_t bus) {
    error_t err = NO_ERROR;

    for(uint8_t device = 0; device < 32; device++) {
        // this will init the device with function 0, will also check to see if has any other functions to initialize
        CHECK_AND_RETHROW(init_pci_device(segment, bus, device, 0));
    }

cleanup:
    return err;
}

/**
 * Will init the pci function
 *
 * if this is a bridge we will also recursively iterate it
 *
 * @param segment
 * @param bus
 * @param device
 * @param function
 */
static error_t init_pci_device(uint16_t segment, uint8_t bus, uint8_t device, uint8_t function) {
    error_t err = NO_ERROR;
    pci_dev_t* dev = NULL;

    // get the base address
    char* mmio = NULL;
    CHECK_AND_RETHROW(pci_get_and_map_mmio(segment, bus, device, function, &mmio));

    // check if valid
    if(POKE16(mmio + PCI_REG_VENDOR_ID) == 0xFFFF) {
        // no such device
        vmm_unmap(kernel_address_space, mmio);
        goto cleanup;
    }

    // initial setup
    dev = kmalloc(sizeof(pci_dev_t));
    dev->segment = segment;
    dev->bus = bus;
    dev->device = device;
    dev->function = function;
    dev->mmio = mmio;
    dev->vendor_id = pci_read_16(dev, PCI_REG_VENDOR_ID);
    dev->device_id = pci_read_16(dev, PCI_REG_DEVICE_ID);
    dev->class = pci_read_8(dev, PCI_REG_CLASS_CODE);
    dev->subclass = pci_read_8(dev, PCI_REG_SUBCLASS_CODE);
    dev->prog_if = pci_read_8(dev, PCI_REG_PROGRAM_INTERFACE);

    // log it
    log_info("\t%x.%x.%x.%x (0x%llx) -> %s (%x:%x)",
            segment, bus, device, function,
            dev->mmio - DIRECT_MAPPING_BASE,
            pci_get_name(dev),
            dev->vendor_id, dev->device_id);

    // set the parent if there is one
    if(arrlen(bus_stack) > 0) {
        dev->parent = arrlast(bus_stack);
    }

     // TODO: check the caps

     // get the bars depending on the header
    uint8_t header_type = (uint8_t) (pci_read_8(dev, PCI_REG_HEADER_TYPE) & ~0x80);
    if(header_type == 1) {
        // TODO: check the bars
    } else if (header_type == 0) {
        // TODO: check the bars
    }

    // check irq
    CHECK_AND_RETHROW(route_device_irq(dev));

    // function scanning
    if(dev->class == PCI_CLASS_BRIDGE_DEVICE && dev->subclass == PCI_SUBCLASS_HOST_BRIDGE) {
        /*
         * For the host bridge each of the functions is a possible bus
         */
        bool multi_function = (bool) (pci_read_8(dev, PCI_REG_HEADER_TYPE) & 0x80);
        if(multi_function) {
            // scan each of the other functions
            for(int i = 1; i < 8; i++) {
                CHECK_AND_RETHROW(scan_bus(segment, i));
            }
        }
    }else {
        /*
         * For a normal device each function is a
         * different device
         */
        if(function == 0) {
            for(int i = 1; i < 8; i++) {
                CHECK_AND_RETHROW(init_pci_device(segment, bus, device, i));
            }
        }
    }

    /*
     * If this is a pci to pci bridge we should
     * scan that secondary bus
     */
    if(dev->class == PCI_CLASS_BRIDGE_DEVICE && dev->subclass == PCI_SUBCLASS_PCI_TO_PCI_BRIDGE) {
        // push this as the parent
        arrpush(bus_stack, dev);

        // initialize the secondary bus
        uint8_t new_bus = pci_read_8(dev, PCI_BRIDGE_REG_SECONDARY_BUS_NUMBER);
        CHECK_AND_RETHROW(scan_bus(segment, new_bus));

        // pop this since this is no longer the parent
        arrpop(bus_stack);
    }

    // add to the device list :)
    arrpush(pci_devices, dev);

cleanup:
    if(err != NO_ERROR) {
        if(dev) kfree(dev);
        vmm_unmap(kernel_address_space, mmio);
    }
    return err;
}

//////////////////////////////////////////
// Init
//////////////////////////////////////////

error_t pci_init() {
    error_t err = NO_ERROR;

    // init the host bridge (always at 0.0.0.0)
    // this will recursively init all other devices as well
    for(int i = 0; i < ((mcfg->header.length - sizeof(mcfg_t)) / sizeof(mcfg_entry_t)); i++) {
        mcfg_entry_t *entry = &mcfg->entries[i];
        CHECK_AND_RETHROW(scan_bus(entry->segment, entry->start_pci_bus));
    }

cleanup:
    return err;
}

//////////////////////////////////////////
// The read and write functions
//////////////////////////////////////////

uint64_t pci_read_64(pci_dev_t* dev, uint16_t offset) {
    return *(uint64_t*)&dev->mmio[offset];
}

uint32_t pci_read_32(pci_dev_t* dev, uint16_t offset) {
    return *(uint32_t*)&dev->mmio[offset];
}

uint16_t pci_read_16(pci_dev_t* dev, uint16_t offset) {
    return *(uint16_t*)&dev->mmio[offset];
}

uint8_t pci_read_8(pci_dev_t* dev, uint16_t offset) {
    return (uint8_t)dev->mmio[offset];
}

void pci_write_64(pci_dev_t* dev, uint16_t offset, uint64_t value) {
    *(uint64_t*)&dev->mmio[offset] = value;
}

void pci_write_32(pci_dev_t* dev, uint16_t offset, uint32_t value) {
    *(uint32_t*)&dev->mmio[offset] = value;
}

void pci_write_16(pci_dev_t* dev, uint16_t offset, uint16_t value) {
    *(uint16_t*)&dev->mmio[offset] = value;
}

void pci_write_8(pci_dev_t* dev, uint16_t offset, uint8_t value) {
    dev->mmio[offset] = value;
}

/////////////////////////////////////////////
// Helper functions
/////////////////////////////////////////////

error_t pci_get_and_map_mmio(uint16_t segment, uint8_t bus, uint8_t device, uint8_t function, char** mmio) {
    error_t err = NO_ERROR;

    CHECK(mcfg);
    CHECK(mmio);

    for(int i = 0; i < ((mcfg->header.length - sizeof(mcfg_t)) / sizeof(mcfg_entry_t)); i++) {
        mcfg_entry_t* entry = &mcfg->entries[i];
        if(entry->segment == segment && entry->start_pci_bus <= bus && entry->end_pci_bus >= bus) {
            uintptr_t phys = entry->base + (((bus - entry->start_pci_bus) << 20) | (device << 15) | function << 12);
            if(!vmm_is_mapped(kernel_address_space, CONVERT_TO_DIRECT(phys))) {
                CHECK_AND_RETHROW(vmm_map(kernel_address_space, (void*)CONVERT_TO_DIRECT(phys), (void*)phys, PAGE_ATTR_WRITE));
            }
            *mmio = (char*)CONVERT_TO_DIRECT(phys);
            goto cleanup;
        }
    }

cleanup:
    return err;
}