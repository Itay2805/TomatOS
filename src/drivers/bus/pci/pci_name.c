
#include <libc/stdint.h>

const char* pci_get_name(uint8_t class, uint8_t subclass, uint8_t prog_if) {
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
        case 0x04: {
            switch(subclass) {
                case 0x00: return "Multimedia Video Controller";
                case 0x01: return "Multimedia Audio Controller";
                case 0x02: return "Computer Telephony Device";
                case 0x03: return "Audio Device";
                default: return "Multimedia Controller";
            }
        }
        case 0x05:
            switch(subclass) {
                case 0x00: return "RAM Controller";
                case 0x01: return "Flash Controller";
                default: return "Memory Controller";
            }
        case 0x06: {
            switch(subclass) {
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
            switch(subclass) {
                case 0x00: {
                    switch(prog_if) {
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
                    switch(prog_if) {
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
            switch(subclass) {
                case 0x00:
                    switch(prog_if) {
                        case 0x00: return "PIC 8259";
                        case 0x01: return "ISA PIC";
                        case 0x02: return "EISA PIC";
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
                        default: return "RTC";
                    }
                case 0x04: return "PCI Hot-Plug Controller";
                case 0x05: return "SD Host Controller";
                case 0x06: return "IOMMU";
                default: return "System peripheral";
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
        case 0x0D: {
            switch(subclass) {
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
            switch(subclass) {
                case 0x00: return "Intelligent Controller (I20)";
                default: return "Intelligent Controller";
            }
        case 0x0F: return "Satellite Communication Controller";
        case 0x10: return "Encryption Controller";
        case 0x11: {
            switch(subclass) {
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
