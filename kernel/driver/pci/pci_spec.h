#ifndef __TOMATOS_KERNEL_SYS_PCI_PCI_SPEC_H__
#define __TOMATOS_KERNEL_SYS_PCI_PCI_SPEC_H__

#include <util/defs.h>
#include <stdint.h>
#include <acpispec/tables.h>

typedef struct pci_command {
    uint16_t io_space : 1;
    uint16_t memory_space : 1;
    uint16_t bus_master : 1;
    uint16_t special_cycle : 1;
    uint16_t memory_write_and_invalidate : 1;
    uint16_t vga_palette_snoop : 1;
    uint16_t parity_error_respond : 1;
    uint16_t stepping_control : 1;
    uint16_t serr : 1;
    uint16_t fast_back_to_back : 1;
} PACKED pci_command_t;

typedef struct pci_status {
    uint16_t _reserved1 : 4;
    uint16_t capability : 1;
    uint16_t capable_66mz : 1;
    uint16_t _reserved2 : 1;
    uint16_t fast_back_to_back_capable : 1;
    uint16_t master_data_parity_error : 1;
} PACKED pci_status_t;

typedef struct pci_common_header {
    uint16_t vendor_id;
    uint16_t device_id;
    pci_command_t command;
    pci_status_t status;
    uint8_t revision_id;
    uint8_t progif;
    uint8_t subclass;
    uint8_t class;
    uint8_t cache_line_size;
    uint8_t latency_timer;
    uint8_t header_type;
    uint8_t bist;
} PACKED pci_common_header_t;

typedef struct pci_device_header {
    uint32_t bar[6];
    uint32_t cisptr;
    uint16_t subsystem_vendor_id;
    uint16_t subsystem_id;
    uint32_t expansion_rom;
    uint8_t capability_ptr;
    uint8_t _reserved[3];
    uint32_t _reserved2;
    uint8_t interrupt_line;
    uint8_t interrupt_pin;
    uint8_t min_gnt;
    uint8_t max_lat;
} PACKED pci_device_header_t;

typedef struct pci_bridge_command {
    uint16_t parity_error_response : 1;
    uint16_t serr : 1;
    uint16_t isa : 1;
    uint16_t vga : 1;
    uint16_t vga_16 : 1;
    uint16_t master_abort : 1;
    uint16_t reset_secondary_bus : 1;
    uint16_t fast_back_to_back : 1;
    uint16_t primary_disacrd_timer : 1;
    uint16_t secondary_discard_timer : 1;
    uint16_t timer_status : 1;
    uint16_t discard_timer_serr : 1;
} PACKED pci_bridge_command_t;

typedef struct pci_bridge_header {
    uint32_t bar[2];
    uint8_t primary_bus;
    uint8_t secondary_bus;
    uint8_t subordinate_bus;
    uint8_t secondary_latency_timer;
    uint8_t io_base;
    uint8_t io_limit;
    uint16_t secondary_status;
    uint16_t memory_base;
    uint16_t memory_limit;
    uint16_t prefetchable_memory_base;
    uint16_t prefetchable_memory_limit;
    uint32_t prefetchable_base_upper32;
    uint32_t prefetchable_limit_upper32;
    uint16_t io_base_upper16;
    uint16_t io_limit_upper16;
    uint8_t _reserved[3];
    uint32_t expansion_bar;
    uint8_t interrupt_line;
    uint8_t interrupt_pin;
    uint16_t bridge_control;
} PACKED pci_bridge_header_t;

#define PCI_CLASS_OLD                 0x00
#define   PCI_CLASS_OLD_OTHER           0x00
#define   PCI_CLASS_OLD_VGA             0x01

#define PCI_CLASS_MASS_STORAGE        0x01
#define   PCI_CLASS_MASS_STORAGE_SCSI   0x00
#define   PCI_CLASS_MASS_STORAGE_IDE    0x01
#define   PCI_CLASS_MASS_STORAGE_FLOPPY 0x02
#define   PCI_CLASS_MASS_STORAGE_IPI    0x03
#define   PCI_CLASS_MASS_STORAGE_RAID   0x04
#define   PCI_CLASS_MASS_STORAGE_OTHER  0x80

#define PCI_CLASS_NETWORK             0x02
#define   PCI_CLASS_NETWORK_ETHERNET    0x00
#define   PCI_CLASS_NETWORK_TOKENRING   0x01
#define   PCI_CLASS_NETWORK_FDDI        0x02
#define   PCI_CLASS_NETWORK_ATM         0x03
#define   PCI_CLASS_NETWORK_ISDN        0x04
#define   PCI_CLASS_NETWORK_OTHER       0x80

#define PCI_CLASS_DISPLAY             0x03
#define   PCI_CLASS_DISPLAY_VGA         0x00
#define     PCI_IF_VGA_VGA                0x00
#define     PCI_IF_VGA_8514               0x01
#define   PCI_CLASS_DISPLAY_XGA         0x01
#define   PCI_CLASS_DISPLAY_3D          0x02
#define   PCI_CLASS_DISPLAY_OTHER       0x80

#define PCI_CLASS_MEDIA               0x04
#define   PCI_CLASS_MEDIA_VIDEO         0x00
#define   PCI_CLASS_MEDIA_AUDIO         0x01
#define   PCI_CLASS_MEDIA_TELEPHONE     0x02
#define   PCI_CLASS_MEDIA_OTHER         0x80

#define PCI_CLASS_MEMORY_CONTROLLER   0x05
#define   PCI_CLASS_MEMORY_RAM          0x00
#define   PCI_CLASS_MEMORY_FLASH        0x01
#define   PCI_CLASS_MEMORY_OTHER        0x80

#define PCI_CLASS_BRIDGE              0x06
#define   PCI_CLASS_BRIDGE_HOST         0x00
#define   PCI_CLASS_BRIDGE_ISA          0x01
#define   PCI_CLASS_BRIDGE_EISA         0x02
#define   PCI_CLASS_BRIDGE_MCA          0x03
#define   PCI_CLASS_BRIDGE_P2P          0x04
#define     PCI_IF_BRIDGE_P2P             0x00
#define     PCI_IF_BRIDGE_P2P_SUBTRACTIVE 0x01
#define   PCI_CLASS_BRIDGE_PCMCIA       0x05
#define   PCI_CLASS_BRIDGE_NUBUS        0x06
#define   PCI_CLASS_BRIDGE_CARDBUS      0x07
#define   PCI_CLASS_BRIDGE_RACEWAY      0x08
#define   PCI_CLASS_BRIDGE_OTHER        0x80
#define   PCI_CLASS_BRIDGE_ISA_PDECODE  0x80

#define PCI_CLASS_SCC                 0x07  ///< Simple communications controllers
#define   PCI_SUBCLASS_SERIAL           0x00
#define     PCI_IF_GENERIC_XT             0x00
#define     PCI_IF_16450                  0x01
#define     PCI_IF_16550                  0x02
#define     PCI_IF_16650                  0x03
#define     PCI_IF_16750                  0x04
#define     PCI_IF_16850                  0x05
#define     PCI_IF_16950                  0x06
#define   PCI_SUBCLASS_PARALLEL         0x01
#define     PCI_IF_PARALLEL_PORT          0x00
#define     PCI_IF_BI_DIR_PARALLEL_PORT   0x01
#define     PCI_IF_ECP_PARALLEL_PORT      0x02
#define     PCI_IF_1284_CONTROLLER        0x03
#define     PCI_IF_1284_DEVICE            0xFE
#define   PCI_SUBCLASS_MULTIPORT_SERIAL 0x02
#define   PCI_SUBCLASS_MODEM            0x03
#define     PCI_IF_GENERIC_MODEM          0x00
#define     PCI_IF_16450_MODEM            0x01
#define     PCI_IF_16550_MODEM            0x02
#define     PCI_IF_16650_MODEM            0x03
#define     PCI_IF_16750_MODEM            0x04
#define   PCI_SUBCLASS_SCC_OTHER        0x80

#define PCI_CLASS_SYSTEM_PERIPHERAL   0x08
#define   PCI_SUBCLASS_PIC              0x00
#define     PCI_IF_8259_PIC               0x00
#define     PCI_IF_ISA_PIC                0x01
#define     PCI_IF_EISA_PIC               0x02
#define     PCI_IF_APIC_CONTROLLER        0x10  ///< I/O APIC interrupt controller , 32 byte none-prefetchable memory.
#define     PCI_IF_APIC_CONTROLLER2       0x20
#define   PCI_SUBCLASS_DMA              0x01
#define     PCI_IF_8237_DMA               0x00
#define     PCI_IF_ISA_DMA                0x01
#define     PCI_IF_EISA_DMA               0x02
#define   PCI_SUBCLASS_TIMER            0x02
#define     PCI_IF_8254_TIMER             0x00
#define     PCI_IF_ISA_TIMER              0x01
#define     PCI_IF_EISA_TIMER             0x02
#define   PCI_SUBCLASS_RTC              0x03
#define     PCI_IF_GENERIC_RTC            0x00
#define     PCI_IF_ISA_RTC                0x01
#define   PCI_SUBCLASS_PNP_CONTROLLER   0x04    ///< HotPlug Controller
#define   PCI_SUBCLASS_PERIPHERAL_OTHER 0x80

#define PCI_CLASS_INPUT_DEVICE        0x09
#define   PCI_SUBCLASS_KEYBOARD         0x00
#define   PCI_SUBCLASS_PEN              0x01
#define   PCI_SUBCLASS_MOUSE_CONTROLLER 0x02
#define   PCI_SUBCLASS_SCAN_CONTROLLER  0x03
#define   PCI_SUBCLASS_GAMEPORT         0x04
#define     PCI_IF_GAMEPORT               0x00
#define     PCI_IF_GAMEPORT1              0x10
#define   PCI_SUBCLASS_INPUT_OTHER      0x80

#define PCI_CLASS_DOCKING_STATION     0x0A
#define   PCI_SUBCLASS_DOCKING_GENERIC  0x00
#define   PCI_SUBCLASS_DOCKING_OTHER    0x80

#define PCI_CLASS_PROCESSOR           0x0B
#define   PCI_SUBCLASS_PROC_386         0x00
#define   PCI_SUBCLASS_PROC_486         0x01
#define   PCI_SUBCLASS_PROC_PENTIUM     0x02
#define   PCI_SUBCLASS_PROC_ALPHA       0x10
#define   PCI_SUBCLASS_PROC_POWERPC     0x20
#define   PCI_SUBCLASS_PROC_MIPS        0x30
#define   PCI_SUBCLASS_PROC_CO_PORC     0x40 ///< Co-Processor

#define PCI_CLASS_SERIAL              0x0C
#define   PCI_CLASS_SERIAL_FIREWIRE     0x00
#define     PCI_IF_1394                   0x00
#define     PCI_IF_1394_OPEN_HCI          0x10
#define   PCI_CLASS_SERIAL_ACCESS_BUS   0x01
#define   PCI_CLASS_SERIAL_SSA          0x02
#define   PCI_CLASS_SERIAL_USB          0x03
#define     PCI_IF_UHCI                   0x00
#define     PCI_IF_OHCI                   0x10
#define     PCI_IF_USB_OTHER              0x80
#define     PCI_IF_USB_DEVICE             0xFE
#define   PCI_CLASS_SERIAL_FIBRECHANNEL 0x04
#define   PCI_CLASS_SERIAL_SMB          0x05

#define PCI_CLASS_WIRELESS            0x0D
#define   PCI_SUBCLASS_IRDA             0x00
#define   PCI_SUBCLASS_IR               0x01
#define   PCI_SUBCLASS_RF               0x10
#define   PCI_SUBCLASS_WIRELESS_OTHER   0x80

#define PCI_CLASS_INTELLIGENT_IO      0x0E

#define PCI_CLASS_SATELLITE           0x0F
#define   PCI_SUBCLASS_TV               0x01
#define   PCI_SUBCLASS_AUDIO            0x02
#define   PCI_SUBCLASS_VOICE            0x03
#define   PCI_SUBCLASS_DATA             0x04

#define PCI_SECURITY_CONTROLLER       0x10   ///< Encryption and decryption controller
#define   PCI_SUBCLASS_NET_COMPUT       0x00
#define   PCI_SUBCLASS_ENTERTAINMENT    0x10
#define   PCI_SUBCLASS_SECURITY_OTHER   0x80

#define PCI_CLASS_DPIO                0x11
#define   PCI_SUBCLASS_DPIO             0x00
#define   PCI_SUBCLASS_DPIO_OTHER       0x80

typedef struct acpi_mcfg_entry {
    uint64_t base_address;
    uint16_t pci_segment;
    uint8_t start_bus;
    uint8_t end_bus;
    uint32_t _reserved;
} PACKED acpi_mcfg_entry_t;

typedef struct acpi_mcfg {
    acpi_header_t header;
    uint64_t _reserved;
    acpi_mcfg_entry_t entries[0];
} PACKED acpi_mcfg_t;

#endif //__TOMATOS_KERNEL_SYS_PCI_PCI_SPEC_H__
