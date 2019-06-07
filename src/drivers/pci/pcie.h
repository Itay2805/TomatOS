#ifndef TOMATKERNEL_PCIE_H
#define TOMATKERNEL_PCIE_H

#include <stdint.h>
#include <stdbool.h>
#include <error.h>

/**
 * Includes whatever is needed to communicate with pcie device
 */
typedef struct pcidev_pcie {
    uint8_t bus;
    uint8_t device;
    uint8_t function;

    char * mmio_base;
} pcidev_pcie_t;

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

// forward declare
struct pcidev;

uint64_t pcie_config_read_64(struct pcidev* dev, uint16_t offset);
uint32_t pcie_config_read_32(struct pcidev* dev, uint16_t offset);
uint16_t pcie_config_read_16(struct pcidev* dev, uint16_t offset);
uint8_t pcie_config_read_8(struct pcidev* dev, uint16_t offset);

#endif //TOMATKERNEL_PCIE_H
