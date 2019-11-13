#ifndef TOMATKERNEL_PCI_H
#define TOMATKERNEL_PCI_H

#include <objects/object.h>

typedef struct pci_device {
    object_t _;

    uint16_t segment;
    uint8_t bus;
    uint8_t slot;
    uint8_t function;

} pci_device_t;

const void* PciDevice();


/**
 * Will get the configuration space base address for the given pci device
 *
 * Will also map it if needed
 *
 * @param seg       [IN] The segment
 * @param bus       [IN] The bus
 * @param slot      [IN] The slot
 * @param function  [IN] The function
 */
void* pci_get_config_space(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t function);

#endif //TOMATKERNEL_PCI_H
