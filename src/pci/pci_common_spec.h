#ifndef TOMATKERNEL_PCI_COMMON_SPEC_H
#define TOMATKERNEL_PCI_COMMON_SPEC_H

#include <stdint.h>

// common registers
#define PCI_REG_VENDOR_ID                                   0x00
#define PCI_REG_DEVICE_ID                                   0x02
#define PCI_REG_COMMAND                                     0x04
#define PCI_REG_STATUS                                      0x06
#define PCI_REG_REVISION_ID                                 0x08
#define PCI_REG_PROGRAM_INTERFACE                           0x09
#define PCI_REG_SUBCLASS_CODE                               0x0A
#define PCI_REG_CLASS_CODE                                  0x0B
#define PCI_REG_CACHE_LINE_SIZE                             0x0C
#define PCI_REG_MASTER_LATENCY_TIMER                        0x0D
#define PCI_REG_HEADER_TYPE                                 0x0E
#define PCI_REG_BIST                                        0x0F
#define PCI_REG_CAPS_POINTER                                0x34
#define PCI_REG_INTERRUPT_LINE                              0x3C
#define PCI_REG_INTERRUPT_PIN                               0x3D

// common command register bits
#define PCI_COMMAND_BUS_MASTER_ENABLE                       BIT(2)
#define PCI_COMMAND_SPECIAL_CYCLE_ENABLE                    BIT(3)
#define PCI_COMMAND_MEMORY_WRITE_AND_INVALIDATE             BIT(4)
#define PCI_COMMAND_VGA_PALETTE_SNOOP                       BIT(5)
#define PCI_COMMAND_PARITY_ERROR_RESPONSE                   BIT(6)
#define PCI_COMMAND_IDSEL_STEPPING_WAIT_CYCLE_CONTROL       BIT(7)
#define PCI_COMMAND_SERR_ENABLE                             BIT(8)
#define PCI_COMMAND_FAST_BACK_TO_BACK_TRANSACTIONS_ENABLE   BIT(9)
#define PCI_COMMAND_INETERRUPT_DISABLE                      BIT(10)

// common status register bits
#define PCI_STATUS_INTERRUPT                                BIT(3)
#define PCI_STATUS_CAPS_LIST                                BIT(4)
#define PCI_STATUS_66MHZ_CAPABLE                            BIT(5)
#define PCI_STATUS_FAST_BACK_TO_BACK_TRANSACTIONS_CAPABLE   BIT(7)
#define PCI_STATUS_MASTER_DATA_PARITY_ERROR                 BIT(8)
#define PCI_STATUS_DEVSEL_TIMING                            (BIT(9) | BIT(10))
#define PCI_STATUS_SIGNALED_TARGET_ABORT                    BIT(11)
#define PCI_STATUS_RECEIVED_TARGET_ABORT                    BIT(12)
#define PCI_STATUS_RECEVIED_MASTER_ABORT                    BIT(13)
#define PCI_STATUS_SIGNALED_SYSTEM_ERROR                    BIT(14)
#define PCI_STATUS_DETECTED_PARITY_ERROR                    BIT(15)



#endif //TOMATKERNEL_PCI_COMMON_SPEC_H
