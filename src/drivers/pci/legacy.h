#ifndef TOMATKERNEL_LEGACY_H
#define TOMATKERNEL_LEGACY_H

#include <stdbool.h>
#include <error.h>

/**
 * Includes whatever is needed to communicate with legacy pci device
 */
typedef struct pcidev_legacy {
    uint8_t bus;
    uint8_t slot;
    uint8_t function;
} pcidev_legacy_t;

/**
 * Will check if legacy PCI is supported
 * @return
 */
bool pci_legacy_supported();

/**
 * Initialize Legacy PCI
 *
 * @remark
 * This assumes that Legacy PCI is supported
 */
error_t pci_legacy_init();

// forward declare
struct pcidev;

uint64_t pci_legacy_config_read_64(struct pcidev* dev, uint16_t offset);
uint32_t pci_legacy_config_read_32(struct pcidev* dev, uint16_t offset);
uint16_t pci_legacy_config_read_16(struct pcidev* dev, uint16_t offset);
uint8_t pci_legacy_config_read_8(struct pcidev* dev, uint16_t offset);


#endif //TOMATKERNEL_LEGACY_H
