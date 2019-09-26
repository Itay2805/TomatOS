#ifndef TOMATKERNEL_AHCI_H
#define TOMATKERNEL_AHCI_H

#include <objects/object.h>
#include <error.h>

// TODO: SATAPI

typedef enum ahci_port_type {
    AHCI_PORT_INVALID,
    AHCI_PORT_SATA,
    AHCI_PORT_SATAPI
} ahci_port_type_t;

typedef struct ahci_port {
    object_t* obj;

    // ahci related
    void* command_list_base;
    void* recevied_fis_base;

    // specific for type
    ahci_port_type_t type;
    union {
        struct {
        } sata;
        struct {
        } satapi;
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
