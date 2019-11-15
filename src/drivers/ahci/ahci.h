#ifndef TOMATKERNEL_AHCI_H
#define TOMATKERNEL_AHCI_H

#include <objects/object.h>
#include <drivers/pci/pci.h>

#include "ahci_spec.h"

typedef struct ahci_device {
    object_t _;

    struct ahci_controller* controller;
    AHCI_HBA_PORT* port;

    lock_t slots_lock;
    bool slot_in_use[32];
    event_t interrupt_on_slot[32];

    AHCI_HBA_CMD_HEADER* cl;
    AHCI_HBA_FIS* fis;

    size_t block_size;

    // signalled whenever there is a new empty slot
    event_t has_empty_slot;
} ahci_device_t;

typedef struct ahci_controller {
    object_t _;

    pci_device_t* device;
    interrupt_handler_t interrupt;

    AHCI_HBA* hba;

    ahci_device_t* devices[32];
} ahci_controller_t;

const void* AhciDevice();

const void* AhciController();

void ahci_scan();

#endif //TOMATKERNEL_AHCI_H
