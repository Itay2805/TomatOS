#ifndef TOMATKERNEL_PCIE_H
#define TOMATKERNEL_PCIE_H

#include <stdint.h>
#include <stdbool.h>
#include <error.h>
#include "pci.h"

/**
 * Will check if PCIe is supported
 * @return
 */
bool pcie_supported();

/**
 * Initialize PCIe
 *
 * @remark
 * This assumes that PCIe is supported
 */
error_t pcie_init();

uint64_t pcie_config_read_64(pcidev_t* dev, uint16_t offset);
uint32_t pcie_config_read_32(pcidev_t* dev, uint16_t offset);
uint16_t pcie_config_read_16(pcidev_t* dev, uint16_t offset);
uint8_t pcie_config_read_8(pcidev_t* dev, uint16_t offset);

void pcie_config_write_64(pcidev_t* dev, uint16_t offset, uint64_t value);
void pcie_config_write_32(pcidev_t* dev, uint16_t offset, uint32_t value);
void pcie_config_write_16(pcidev_t* dev, uint16_t offset, uint16_t value);
void pcie_config_write_8(pcidev_t* dev, uint16_t offset, uint8_t value);

#endif //TOMATKERNEL_PCIE_H
