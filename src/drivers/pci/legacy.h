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
    uint8_t func;
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

#endif //TOMATKERNEL_LEGACY_H
