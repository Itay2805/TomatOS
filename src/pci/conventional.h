#ifndef TOMATKERNEL_LEGACY_H
#define TOMATKERNEL_LEGACY_H

#include <stdbool.h>
#include <error.h>
#include "pci.h"

/**
 * Will check if legacy PCI is supported
 * @return
 */
bool pci_conventional_supported();

/**
 * Initialize Legacy PCI
 *
 * @remark
 * This assumes that Legacy PCI is supported
 */
error_t pci_conventional_init();

uint64_t pci_conventional_config_read_64(pcidev_t *dev, uint16_t offset);
uint32_t pci_conventional_config_read_32(pcidev_t *dev, uint16_t offset);
uint16_t pci_conventional_config_read_16(pcidev_t *dev, uint16_t offset);
uint8_t pci_conventional_config_read_8(pcidev_t *dev, uint16_t offset);

void pci_conventional_config_write_64(pcidev_t *dev, uint16_t offset, uint64_t value);
void pci_conventional_config_write_32(pcidev_t *dev, uint16_t offset, uint32_t value);
void pci_conventional_config_write_16(pcidev_t *dev, uint16_t offset, uint16_t value);
void pci_conventional_config_write_8(pcidev_t *dev, uint16_t offset, uint8_t value);

#endif //TOMATKERNEL_LEGACY_H
