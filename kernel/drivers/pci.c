#include "pci.h"

#include <common/string.h>
#include "portio.h"

#define PCI_REG_ADDRESS     0xcf8
#define PCI_REG_DATA        0xcfc

pci_device_t* pci_devices;

// based on https://wiki.osdev.org/PCI#Class_Codes
static const char* get_device_name(uint16_t class, uint16_t subclass, uint16_t prog_if) {
    switch(class) {
        case 0x00:
            switch(subclass) {
                case 0x00: return "Non-VGA-Compatible device";
                case 0x01: return "VGA-Compatible device";
                default: return "Unclassified";
            }
        case 0x01:
            switch(subclass) {
                case 0x00: return "SCSI Bus Controller";
                case 0x01: return "IDE Controller";
                case 0x02: return "Floppy Disk Controller";
                case 0x03: return "IPI Bus Controller";
                case 0x04: return "RAID Controller";
                case 0x05:
                    switch(prog_if) {
                        case 0x20: return "ATA Controller (Single DMA)";
                        case 0x30: return "ATA Controller (Chained DMA)";
                        default: return "ATA Controller";
                    }
                case 0x06:
                    switch(prog_if) {
                        case 0x00: return "SATA (Vendor Specific)";
                        case 0x01: return "SATA (AHCI 1.0)";
                        case 0x02: return "Serial Storage Bus (SATA)";
                        default: return "Serial ATA";
                    }
                case 0x07:
                    switch(prog_if) {
                        case 0x00: return "SAS";
                        case 0x01: return "Serial Storage Bus (SAS)";
                        default: return "Serial Attached SCSI";
                    }
                case 0x08: 
                    switch(prog_if) {
                        case 0x01: return "NVMHCI";
                        case 0x02: return "NVM Express";
                        default: return "Non-Volatile Memory Controller";
                    }
                default: return "Mass Storage Controller";
            }
        case 0x02:
            switch(subclass) {
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
            switch(subclass) {
                case 0x00:
                    switch(prog_if) {
                        case 0x00: return "VGA Controller";
                        case 0x01: return "8514-Compatible Controller";
                        default: return "VGA-Compatible Controller";
                    }
                case 0x01: return "XGA Controller";
                case 0x02: return "3D Controller (Non VGA-Compatible)";
                default: return "Display Controller";
            }
        case 0x04: return "Multimedia Controller";
        case 0x05:
            switch(subclass) {
                case 0x00: return "RAM Controller";
                case 0x01: return "Flash Controller";
                default: return "Memory Controller";
            }
        case 0x06: return "Bridge Device";
        case 0x07: return "Simple Communication Controller";
        case 0x08:
            switch(subclass) {
                case 0x00:
                    switch(prog_if) {
                        case 0x00: return "PIC (Generic 8259-Compatible)";
                        case 0x01: return "PIC (ISA-Compatible)";
                        case 0x02: return "PIC (EISA-Compatible)";
                        case 0x10: return "I/O APIC Interrupt Controller";
                        case 0x20: return "I/O(x) APIC Interrupt Controller";
                        default: return "PIC";
                    }
                case 0x01:
                    switch(prog_if) {
                        case 0x00: return "DMA Controller (Generic 8254-Compatible)";
                        case 0x01: return "DMA Controller (ISA-Compatible)";
                        case 0x02: return "DMA Controller (EISA-Compatible)";
                        default: return "DMA Controller";
                    }
                case 0x02:
                    switch(prog_if) {
                        case 0x00: return "Timer (Generic 8254-Compatible)";
                        case 0x01: return "Timer (ISA-Compatible)";
                        case 0x02: return "Timer (EISA-Compatible)";
                        case 0x03: return "Timer (HPET)";
                        default: return "Timer";
                    }
                case 0x03:
                    switch(prog_if) {
                        case 0x00: return "Generic RTC";
                        case 0x01: return "RTC (ISA-Compatible)";
                        default: return "RTC Controller";
                    }
                case 0x04: return "PCI Hot-Plug Controller";
                case 0x05: return "SD Host Controller";
                case 0x06: return "IOMMU";
                default: return "Base System Peripheral";
            }
        case 0x09:
            switch(subclass) {
                case 0x00: return "Keyboard Controller";
                case 0x01: return "Defitizer Pen";
                case 0x02: return "Mouse Controller";
                case 0x03: return "Scanner Controller";
                case 0x04: return "Gameport Controller";
                default: return "Input Device Controller";
            }
        case 0xA: return "Docking Station";
        case 0xB:
            switch(subclass) {
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
            switch(subclass) {
                case 0x00:
                    switch(prog_if) {
                        case 0x00: return "FireWire (IEEE 1394) Controller (Generic)";
                        case 0x10: return "FireWire (IEEE 1394) Controller (OHCI)";
                        default: return "FireWire (IEEE 1394) Controller";
                    }
                case 0x01: return "ACCESS bus";
                case 0x02: return "SSA";
                case 0x03: 
                    switch(prog_if) {
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
                    switch(prog_if) {
                        case 0x00: return "IPMI Interface (SMIC)";
                        case 0x01: return "IPMI Interface (Keyboard style)";
                        case 0x02: return "IPMI Interface (Block Transfer)";
                        default: return "IPMI Interface";
                    }
                case 0x08: return "SERCOS Interface (IEC 61491)";
                case 0x09: return "Serial Bus Controller (CANbus)";
                default: return "Serial Bus Controller";
            }
        case 0x0D: return "Wireless Controller";
        case 0x0E:
            switch(subclass) {
                case 0x00: return "Intelligent Controller (I20)";
                default: return "Intelligent Controller";
            }
        case 0x0F: return "Satellite Communication Controller";
        case 0x10: return "Encryption Controller";
        case 0x11: return "Signal Processing Controller";
        case 0x12: return "Processing Accelarator";
        case 0x13: return "Non-Essential Instrumentation";
        case 0x40: return "Co-Processor";
        default:
            return "Unknown";
    }
}

static void print_device(pci_device_t* dev) {
    const char* name = get_device_name(dev->class, dev->subclass, dev->prog_if);
    LOG_INFO("\t%d:%d:%d -> %s (%x:%x:%x)", dev->bus, dev->slot, dev->function, name, dev->class, dev->subclass, dev->prog_if);
}

error_t pci_init() {
    error_t err = NO_ERROR;

    outl(PCI_REG_ADDRESS, 0x80000000);
    CHECK_ERROR(inl(PCI_REG_ADDRESS) == 0x80000000, ERROR_NOT_FOUND);

    LOG_INFO("PCI supported, probing devices");

    for(uint16_t bus = 0; bus < 256u; bus++) {
        for(uint16_t slot = 0; slot < 32u; slot++) {
            for(uint16_t function = 0; function < 8; function++) {
                uint16_t vendor = pci_read(bus, slot, function, 0);
                if(vendor == 0xffff) break;
                uint16_t device = pci_read(bus, slot, function, 2);
                uint16_t revision_prog = pci_read(bus, slot, function, 8);
                uint16_t device_class = pci_read(bus, slot, function, 10);
                buf_push(pci_devices, (pci_device_t){
                    .bus = bus,
                    .slot = slot,
                    .function = function,
                    .vendor = vendor,
                    .device = device,
                    .class = device_class >> 8,
                    .subclass = device_class & 0xff,
                    .prog_if = revision_prog >> 8,
                    .revision_id = revision_prog & 0xff
                });
                print_device(&pci_devices[buf_len(pci_devices) - 1]);
            }
        }
    }

cleanup:
    return err;
}

uint16_t pci_read(uint16_t bus, uint16_t slot, uint16_t func, uint16_t reg) {
    uint32_t addr = (bus << 16) | (slot << 11) | (func << 8) | (reg & 0xfc) | 0x80000000;
    outl(PCI_REG_ADDRESS, addr);
    return inl(PCI_REG_DATA) >> ((reg & 2) * 8) & 0xffff;
}

void pci_write(uint16_t bus, uint16_t slot, uint16_t func, uint16_t reg, uint16_t data) {
    uint32_t addr = (bus << 16) | (slot << 11) | (func << 8) | (reg & 0xfc) | 0x80000000;
    outl(PCI_REG_ADDRESS, addr);
    outl(PCI_REG_DATA, data);    
}