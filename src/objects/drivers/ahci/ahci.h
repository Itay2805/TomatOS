#ifndef TOMATKERNEL_AHCI_H
#define TOMATKERNEL_AHCI_H

#include <error.h>

// TODO: SATAPI

typedef enum ahci_port_type {
    AHCI_PORT_INVALID,
    AHCI_PORT_SATA,
    AHCI_PORT_SATAPI
} ahci_port_type_t;

typedef struct ahci_port {
    ahci_port_type_t type;
    union {
        struct {

        } sata;
    };
} ahci_port_t;

typedef struct ahci_device {
    pci_dev_t* dev;
    pci_bar_t* bar;

    char* command_list;
    char* command_table;
    char* received_fis;


    ahci_port_t ports[32];
} ahci_device_t;

ahci_device_t* ahci_devices;

error_t ahci_init();

#endif //TOMATKERNEL_AHCI_H
