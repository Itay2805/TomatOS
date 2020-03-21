#ifndef __SYS_PCI_SPEC_H__
#define __SYS_PCI_SPEC_H__

#include <stdint.h>

//
// Definitions of PCI class bytes and manipulation macros.
//
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

//
// the definition of Header Type
//
#define HEADER_TYPE_DEVICE            0x00
#define HEADER_TYPE_PCI_TO_PCI_BRIDGE 0x01
#define HEADER_TYPE_CARDBUS_BRIDGE    0x02
#define HEADER_TYPE_MULTI_FUNCTION    0x80

//
// Mask of Header type
//
#define HEADER_LAYOUT_CODE            0x7f

///
/// Rom Base Address in Bridge, defined in PCI-to-PCI Bridge Architecture Specification,
///
#define PCI_BRIDGE_ROMBAR             0x38

#define PCI_MAX_BAR                   0x0006
#define PCI_MAX_CONFIG_OFFSET         0x0100

#define PCI_VENDOR_ID_OFFSET                        0x00
#define PCI_DEVICE_ID_OFFSET                        0x02
#define PCI_COMMAND_OFFSET                          0x04
#define PCI_PRIMARY_STATUS_OFFSET                   0x06
#define PCI_REVISION_ID_OFFSET                      0x08
#define PCI_CLASSCODE_OFFSET                        0x09
#define PCI_CACHELINE_SIZE_OFFSET                   0x0C
#define PCI_LATENCY_TIMER_OFFSET                    0x0D
#define PCI_HEADER_TYPE_OFFSET                      0x0E
#define PCI_BIST_OFFSET                             0x0F
#define PCI_BASE_ADDRESSREG_OFFSET                  0x10
#define PCI_CARDBUS_CIS_OFFSET                      0x28
#define PCI_SVID_OFFSET                             0x2C ///< SubSystem Vendor id
#define PCI_SUBSYSTEM_VENDOR_ID_OFFSET              0x2C
#define PCI_SID_OFFSET                              0x2E ///< SubSystem ID
#define PCI_SUBSYSTEM_ID_OFFSET                     0x2E
#define PCI_EXPANSION_ROM_BASE                      0x30
#define PCI_CAPBILITY_POINTER_OFFSET                0x34
#define PCI_INT_LINE_OFFSET                         0x3C ///< Interrupt Line Register
#define PCI_INT_PIN_OFFSET                          0x3D ///< Interrupt Pin Register
#define PCI_MAXGNT_OFFSET                           0x3E ///< Max Grant Register
#define PCI_MAXLAT_OFFSET                           0x3F ///< Max Latency Register

//
// Base Address registers mask
//
#define PCI_BASE_ADDRESS_SPACE	0x01
#define PCI_BASE_ADDRESS_SPACE_IO 0x01
#define PCI_BASE_ADDRESS_SPACE_MEMORY 0x00
#define PCI_BASE_ADDRESS_MEM_TYPE_MASK 0x06
#define PCI_BASE_ADDRESS_MEM_TYPE_32	0x00
#define PCI_BASE_ADDRESS_MEM_TYPE_1M	0x02
#define PCI_BASE_ADDRESS_MEM_TYPE_64	0x04
#define PCI_BASE_ADDRESS_MEM_PREFETCH	0x08
#define PCI_BASE_ADDRESS_MEM_MASK	(~0x0f)
#define PCI_BASE_ADDRESS_IO_MASK	(~0x03)

//
// defined in PCI-to-PCI Bridge Architecture Specification
//
#define PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET      0x18
#define PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET    0x19
#define PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET  0x1a
#define PCI_BRIDGE_SECONDARY_LATENCY_TIMER_OFFSET   0x1b
#define PCI_BRIDGE_STATUS_REGISTER_OFFSET           0x1E
#define PCI_BRIDGE_CONTROL_REGISTER_OFFSET          0x3E

///
/// Interrupt Line "Unknown" or "No connection" value defined for x86 based system
///
#define PCI_INT_LINE_UNKNOWN                        0xFF

#define PCI_COMMAND_IO_SPACE                        BIT0   ///< 0x0001
#define PCI_COMMAND_MEMORY_SPACE                    BIT1   ///< 0x0002
#define PCI_COMMAND_BUS_MASTER                      BIT2   ///< 0x0004
#define PCI_COMMAND_SPECIAL_CYCLE                   BIT3   ///< 0x0008
#define PCI_COMMAND_MEMORY_WRITE_AND_INVALIDATE     BIT4   ///< 0x0010
#define PCI_COMMAND_VGA_PALETTE_SNOOP               BIT5   ///< 0x0020
#define PCI_COMMAND_PARITY_ERROR_RESPOND            BIT6   ///< 0x0040
#define PCI_COMMAND_STEPPING_CONTROL                BIT7   ///< 0x0080
#define PCI_COMMAND_SERR                            BIT8   ///< 0x0100
#define PCI_COMMAND_FAST_BACK_TO_BACK               BIT9   ///< 0x0200

//
// defined in PCI-to-PCI Bridge Architecture Specification
//
#define PCI_BRIDGE_CONTROL_PARITY_ERROR_RESPONSE    BIT0   ///< 0x0001
#define PCI_BRIDGE_CONTROL_SERR                     BIT1   ///< 0x0002
#define PCI_BRIDGE_CONTROL_ISA                      BIT2   ///< 0x0004
#define PCI_BRIDGE_CONTROL_VGA                      BIT3   ///< 0x0008
#define PCI_BRIDGE_CONTROL_VGA_16                   BIT4   ///< 0x0010
#define PCI_BRIDGE_CONTROL_MASTER_ABORT             BIT5   ///< 0x0020
#define PCI_BRIDGE_CONTROL_RESET_SECONDARY_BUS      BIT6   ///< 0x0040
#define PCI_BRIDGE_CONTROL_FAST_BACK_TO_BACK        BIT7   ///< 0x0080
#define PCI_BRIDGE_CONTROL_PRIMARY_DISCARD_TIMER    BIT8   ///< 0x0100
#define PCI_BRIDGE_CONTROL_SECONDARY_DISCARD_TIMER  BIT9   ///< 0x0200
#define PCI_BRIDGE_CONTROL_TIMER_STATUS             BIT10  ///< 0x0400
#define PCI_BRIDGE_CONTROL_DISCARD_TIMER_SERR       BIT11  ///< 0x0800

//
// Following are the PCI status control bit
//
#define PCI_STATUS_CAPABILITY                       BIT4   ///< 0x0010
#define PCI_STATUS_66MZ_CAPABLE                     BIT5   ///< 0x0020
#define PCI_FAST_BACK_TO_BACK_CAPABLE               BIT7   ///< 0x0080
#define PCI_MASTER_DATA_PARITY_ERROR                BIT8   ///< 0x0100

//
// PCI Capability List IDs and records
//
#define PCI_CAPABILITY_ID_PMI     0x01
#define PCI_CAPABILITY_ID_AGP     0x02
#define PCI_CAPABILITY_ID_VPD     0x03
#define PCI_CAPABILITY_ID_SLOTID  0x04
#define PCI_CAPABILITY_ID_MSI     0x05
#define PCI_CAPABILITY_ID_HOTPLUG 0x06
#define PCI_CAPABILITY_ID_SHPC    0x0C

///
/// Capabilities List Header
/// Section 6.7, PCI Local Bus Specification, 2.2
///
typedef struct {
    uint8_t CapabilityID;
    uint8_t NextItemPtr;
} PCI_CAPABILITY_HDR;

///
/// Message Capability Structure for 32-bit Message Address
/// Section 6.8.1, PCI Local Bus Specification, 2.2
///
typedef struct {
    PCI_CAPABILITY_HDR  Hdr;
    uint16_t MsgCtrlReg;
    uint32_t MsgAddrReg;
    uint16_t MsgDataReg;
} PCI_CAPABILITY_MSI32;

///
/// Message Capability Structure for 64-bit Message Address
/// Section 6.8.1, PCI Local Bus Specification, 2.2
///
typedef struct {
    PCI_CAPABILITY_HDR  Hdr;
    uint16_t MsgCtrlReg;
    uint32_t MsgAddrRegLsdw;
    uint32_t MsgAddrRegMsdw;
    uint16_t MsgDataReg;
} PCI_CAPABILITY_MSI64;

#endif //__SYS_PCI_SPEC_H__
