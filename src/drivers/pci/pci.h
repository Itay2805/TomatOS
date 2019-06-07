#ifndef TOMATKERNEL_PCI_H
#define TOMATKERNEL_PCI_H

#include <stdint.h>
#include <error.h>

#include "pcie.h"
#include "legacy.h"

/**************************************************************
 * PCI types
 **************************************************************/

#define PCI_TYPE_UNSUPPORTED    0
#define PCI_TYPE_PCIE           1
#define PCI_TYPE_LEGACY         2

/**************************************************************
 * Generic PCI offsets
 **************************************************************/

#define PCI_VENDOR_ID           0
#define PCI_DEVICE_ID           2
#define PCI_COMMAND             4
#define PCI_STATUS              6
#define PCI_REVISION_ID         8
#define PCI_PROG_IF             9
#define PCI_SUBCLASS            10
#define PCI_CLASS_CODE          11
#define PCI_HEADER_TYPE         14


/**************************************************************
 * General structures for pci
 **************************************************************/

typedef struct pcidev {
    // must be first
    union {
        pcidev_legacy_t legacy;
        pcidev_pcie_t pcie;
    };

    // some static info per device
    uint16_t vendor_id;
    uint16_t device_id;
    uint8_t class;
    uint8_t subclass;
    uint8_t prog_if;
} pcidev_t;

/**
 * What pci driver are we actually using (PCIe/Legacy PCI)
 */
extern uint8_t pcitype;

/**
 * Stretchy buffer of all pci devices
 */
extern pcidev_t* pcidevs;

/**
 * Initialize the PCI abstraction
 *
 * Supports both PCIe and Legacy PCI
 */
error_t pci_init();

/**
 * Get the name of the pci device
 */
const char* pci_get_name(pcidev_t* dev);

/**************************************************************
 * Helper functions to read from the PCI configuration space
 **************************************************************/

extern uint64_t(*pci_config_read_64)(pcidev_t* dev, uint16_t offset);
extern uint32_t(*pci_config_read_32)(pcidev_t* dev, uint16_t offset);
extern uint16_t(*pci_config_read_16)(pcidev_t* dev, uint16_t offset);
extern uint8_t(*pci_config_read_8)(pcidev_t* dev, uint16_t offset);




#endif //TOMATKERNEL_PCI_H
