#ifndef __SYS_PCI_H__
#define __SYS_PCI_H__

#include <stddef.h>
#include <stdint.h>
#include <util/except.h>

typedef enum pci_bar_type {
    PCI_BAR_IO,
    PCI_BAR_MMIO,
} pci_bar_type_t;

typedef struct pci_bar {
    pci_bar_type_t type;
    uint64_t base_addr;
    size_t size;
} pci_bar_t;

typedef struct pci_cap {
    struct pci_cap* next;
    uint16_t id, type;
    int addr;
} pci_cap_t;

typedef struct pci_dev {
    int segment;
    uint8_t bus, dev, func;
    uint16_t vendor_id, device_id;
    uint16_t device_class;
    pci_bar_t* bars;
    pci_cap_t* caps;
} pci_dev_t;

extern pci_dev_t* g_pci_devices;

extern uint32_t (*pci_read_dword)(pci_dev_t* dev, int pos);
extern uint16_t (*pci_read_word)(pci_dev_t* dev, int pos);
extern uint8_t (*pci_read_byte)(pci_dev_t* dev, int pos);

extern void (*pci_write_dword)(pci_dev_t* dev, int pos, uint32_t data);
extern void (*pci_write_word)(pci_dev_t* dev, int pos, uint16_t data);
extern void (*pci_write_byte)(pci_dev_t* dev, int pos, uint8_t data);

void pci_read_bytes(pci_dev_t* dev, int pos, uint8_t* buf, int len);

/**
 * Initialize PCI globally
 */
err_t init_pci();

#endif //__SYS_PCI_H__
