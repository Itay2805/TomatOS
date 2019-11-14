#ifndef TOMATKERNEL_AHCI_H
#define TOMATKERNEL_AHCI_H

#include <objects/object.h>
#include <drivers/pci/pci.h>

#include "ahci_spec.h"

typedef struct ahci_controller {
    object_t _;

    pci_device_t* device;
    interrupt_handler_t interrupt;

    AHCI_HBA* hba;
} ahci_controller_t;

typedef struct ahci_device {
    object_t _;

    ahci_controller_t* controller;
    AHCI_HBA_PORT* port;

    AHCI_HBA_CMD_HEADER* cl;
    AHCI_HBA_FIS* fis;
} ahci_device_t;

const void* AhciDevice();

const void* AhciController();

void ahci_scan();

#endif //TOMATKERNEL_AHCI_H
