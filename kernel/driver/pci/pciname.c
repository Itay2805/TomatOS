#include "pci.h"

const char* get_pci_name(uint8_t class, uint8_t subclass, uint8_t progif) {
    switch (class) {
        case 0x00: {
            switch (subclass) {
                case 0x00: return "Non-VGA unclassified device";
                case 0x01: return "VGA compatible unclassified device";
                default: return "Unclassified device";
            }
        }

        case 0x01: {
            switch (subclass) {
                case 0x00: return "SCSI storage controller";
                case 0x01: return "IDE interface";
                case 0x02: return "Floppy disk controller";
                case 0x03: return "IPI bus controller";
                case 0x04: return "RAID bus controller";
                case 0x05: return "ATA controller";
                case 0x06: {
                    switch (progif) {
                        case 0x01: return "AHCI 1.0";
                        default: return "SATA controller";
                    }
                }
                case 0x07: return "Serial Attached SCSI controller";
                case 0x08: {
                    switch (progif) {
                        case 0x01: return "NVMHCI";
                        case 0x02: return "NVM Express";
                        default: return "Non-Volatile memory controller";
                    }
                }
                default: return "Mass storage controller";
            }
        }

        case 0x02: {
            switch (subclass) {
                case 0x00: return "Ethernet controller";
                default: return "Network controller";
            }
        }

        case 0x03: {
            switch (subclass) {
                case 0x00: {
                    switch (progif) {
                        case 0x00: return "VGA controller";
                        case 0x01: return "8514 controller";
                        default: return "VGA compatible controller";
                    }
                }
                case 0x01: return "XGA compatible controller";
                case 0x02: return "3D controller";
                default: return "Display controller";
            }
        }

        case 0x04: {
            switch (subclass) {
                case 0x00: return "Multimedia video controller";
                case 0x01: return "Multimedia audio controller";
                case 0x02: return "Computer telephony device";
                case 0x03: return "Audio device";
                default: return "Multimedia controller";
            }
        }

        case 0x05: return "Memory controller";

        case 0x06: {
            switch (subclass) {
                case 0x00: return "Host bridge";
                case 0x01: return "ISA bridge";
                case 0x04: return "PCI bridge";
                default: return "Bridge";
            }
        }

        case 0x07: {
            switch (subclass) {
                case 0x00: {
                    switch (progif) {
                        case 0x00: return "Serial controller <8250>";
                        case 0x01: return "Serial controller <16450>";
                        case 0x02: return "Serial controller <16550>";
                        case 0x03: return "Serial controller <16650>";
                        case 0x04: return "Serial controller <16750>";
                        case 0x05: return "Serial controller <16850>";
                        case 0x06: return "Serial controller <16950";
                        default: return "Serial controller";
                    }
                }
                case 0x01: return "Parallel controller";
                case 0x02: return "Multiport serial controller";
                default: return "Communication controller";
            }
        }

        case 0x08: return "Generic system peripheral";
        case 0x09: return "Input device controller";
        case 0x0a: return "Docking station";
        case 0x0b: return "Processor";

        case 0x0c: {
            switch (subclass) {
                case 0x00: return "USB controller";
                case 0x05: return "SMBus";
                default: return "Serial bus controller";
            }
        }

        case 0x0d: {
            switch (subclass) {
                case 0x11: return "Bluetooth";
                case 0x20: return "802.1a controller";
                case 0x21: return "802.1b controller";
                default: return "Wireless controller";
            }
        }

        case 0x0e: return "Intelligent controller";
        case 0x0f: return "Satellite communication controller";
        case 0x10: return "Encryption controller";
        case 0x11: return "Signal processing accelerators";
        case 0x12: return "Processing accelerators";
        case 0x13: return "Non-Essential Instrumentation";
        case 0x40: return "Coprocessor";
        default: return "Unassigned class";
    }
}
