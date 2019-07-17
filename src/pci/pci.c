#include <common.h>
#include "pci.h"
#include "pcie.h"
#include "conventional.h"

uint8_t pcitype = NULL;
pcidev_t* pcidevs = NULL;

uint64_t(*pci_config_read_64)(pcidev_t* dev, uint16_t offset);
uint32_t(*pci_config_read_32)(pcidev_t* dev, uint16_t offset);
uint16_t(*pci_config_read_16)(pcidev_t* dev, uint16_t offset);
uint8_t(*pci_config_read_8)(pcidev_t* dev, uint16_t offset);

void(*pci_config_write_64)(pcidev_t* dev, uint16_t offset, uint64_t value);
void(*pci_config_write_32)(pcidev_t* dev, uint16_t offset, uint32_t value);
void(*pci_config_write_16)(pcidev_t* dev, uint16_t offset, uint16_t value);
void(*pci_config_write_8)(pcidev_t* dev, uint16_t offset, uint8_t value);

error_t pci_init() {
    error_t err = NO_ERROR;

    if(pcie_supported()) {
        pcitype = PCI_TYPE_PCIE;
        log_notice("PCIe is supported");

        // set the config read/write
        pci_config_read_64 = pcie_config_read_64;
        pci_config_read_32 = pcie_config_read_32;
        pci_config_read_16 = pcie_config_read_16;
        pci_config_read_8 = pcie_config_read_8;

        pci_config_write_64 = pcie_config_write_64;
        pci_config_write_32 = pcie_config_write_32;
        pci_config_write_16 = pcie_config_write_16;
        pci_config_write_8 = pcie_config_write_8;

        // initialize the driver
        CHECK_AND_RETHROW(pcie_init());
    }else if(pci_conventional_supported()) {
        pcitype = PCI_TYPE_LEGACY;
        log_notice("Conventional PCI is supported");

        // set the config read/write
        pci_config_read_64 = pci_conventional_config_read_64;
        pci_config_read_32 = pci_conventional_config_read_32;
        pci_config_read_16 = pci_conventional_config_read_16;
        pci_config_read_8 = pci_conventional_config_read_8;

        pci_config_write_64 = pci_conventional_config_write_64;
        pci_config_write_32 = pci_conventional_config_write_32;
        pci_config_write_16 = pci_conventional_config_write_16;
        pci_config_write_8 = pci_conventional_config_write_8;


        // initialize the driver
        CHECK_AND_RETHROW(pci_conventional_init());
    }else {
        CHECK_FAIL_ERROR_TRACE(ERROR_NOT_IMPLEMENTED, "No PCI controller found!");
    }

cleanup:
    return err;
}

const char* pci_get_name(pcidev_t* dev) {
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
        case 0x07: return "Simple Communication Controller";
        case 0x08:
            switch(dev->subclass) {
                case 0x00:
                    switch(dev->prog_if) {
                        case 0x00: return "PIC (Generic 8259-Compatible)";
                        case 0x01: return "PIC (ISA-Compatible)";
                        case 0x02: return "PIC (EISA-Compatible)";
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
                        default: return "RTC Controller";
                    }
                case 0x04: return "PCI Hot-Plug Controller";
                case 0x05: return "SD Host Controller";
                case 0x06: return "IOMMU";
                default: return "Base System Peripheral";
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

const char* pci_get_vendor_name(pcidev_t* dev) {
    switch(dev->vendor_id) {
        case 0x8086: return "Intel";
        case 0x10dE: return "NVidia Corporation";
        case 0x1022: return "Advanced Micro Devices";
        case 0x1414: return "Microsoft";
        case 0x106B: return "Apple Computer";
        case 0x1043: return "Asustek Computer, Inc.";
        case 0x1458: return "Giga-Byte Technology Co., Ltd.";
        case 0x15AD: return "Vmware, Inc.";
        case 0x108E: return "Oracle";
        case 0x1028: return "Dell Computer Corporation";
        case 0x103C: return "Hewlett Packard";
        case 0x1590: return "Hewlett Packard Enterprise";
        case 0x10EC: return "Realtek Semiconductor Corporation";
        case 0x101E: return "American Megatrends Inc.";
        case 0x1AE0: return "Google, Inc.";
        case 0x1234: return "Qemu (Unofficial)";
        default: return NULL;
    }
}
