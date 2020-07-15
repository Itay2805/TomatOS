#ifndef __TOMATOS_KERNEL_SYS_PCI_PCI_H__
#define __TOMATOS_KERNEL_SYS_PCI_PCI_H__

#include <util/except.h>
#include <stdint.h>

/**
 * First pci initialization, no iteration yet
 */
err_t init_pci();

/**
 * Will get the mmio regions for the given pci address
 */
err_t get_config_for_pci(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, void** config);

#endif //__TOMATOS_KERNEL_SYS_PCI_PCI_H__
