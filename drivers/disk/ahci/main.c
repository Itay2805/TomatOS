#include <driver/pci/pci_spec.h>
#include <driver/driver.h>
#include <proc/process.h>
#include <util/defs.h>
#include <proc/scheduler.h>
#include <mem/mm.h>
#include <mem/pmm.h>
#include <stdnoreturn.h>
#include "ahci_spec.h"

#undef __MODULE__
#define __MODULE__ "ahci"

typedef struct ahci_interface {
    driver_instance_t instance;
    volatile hba_port_t* port;
    hba_cmd_header_t* cmd_list;
} ahci_interface_t;

static void ahci_thread(ahci_interface_t* interface) {
    err_t err = NO_ERROR;
    volatile hba_port_t* port = interface->port;

    TRACE("initializing %s", g_current_thread->name);

    // make sure to stop the cmd before doing anything
    port->cmd &= ~HBA_PxCMD_ST;
    port->cmd &= ~HBA_PxCMD_FRE;
    while (1) {
        if (!(port->cmd & HBA_PxCMD_FR) && !(port->cmd & HBA_PxCMD_CR)) {
            break;
        }
    }

    // TODO: this is wasteful on space ig
    void* base;
    CHECK_AND_RETHROW(pmm_allocate_zero(1, &base));

    // set the command list and fis
    interface->port->clb = DIRECT_TO_PHYSICAL(base);
    interface->cmd_list = base;
    interface->port->fb = DIRECT_TO_PHYSICAL(base + SIZE_1KB);

    // set all entries to be zero
    for (int i = 0; i < 32; i++) {
        interface->cmd_list[i].prdtl = 0;
        interface->cmd_list[i].ctba = 0;
    }

    // start the command engine
    while (port->cmd & HBA_PxCMD_CR)
        cpu_pause();
    port->cmd |= HBA_PxCMD_FRE;
    port->cmd |= HBA_PxCMD_ST;

    // TODO: create interrupt handling thread
//        snprintf(thread_name, sizeof(thread_name), "ahci/%04x:%02x:%02x.%x/%d-interrupt",
//                 data->pci_dev->address.seg, data->pci_dev->address.bus,
//                 data->pci_dev->address.slot, data->pci_dev->address.func, i);

    // register the interface
    CHECK_AND_RETHROW(register_interface((void*)interface));

    // we are ready to accept requests
    TRACE("%s ready", g_current_thread->name);
    while (1) {
        yield();
    }

cleanup:
    (void)err;
}

static err_t ahci_entry(driver_bind_data_t* data) {
    err_t err = NO_ERROR;

    // check the device
    CHECK(data != NULL);
    CHECK(data->bind->type == BIND_PCI);

    // sanity check the bar
    CHECK(data->pci_dev->bars[5].present);
    CHECK(data->pci_dev->bars[5].mmio);

    // get the hba
    hba_mem_t* hba = data->pci_dev->bars[5].addr;

    // iterate ports
    TRACE("Iterating ports");
    uint32_t pi = hba->pi;
    for (int i = 0; i < 32; i++) {
        if (!(pi & (1u << i))) continue;

        hba_port_t* port = &hba->ports[i];
        if (port->det != 3 || port->ipm != 1) {
            TRACE("\tport #%d - inactive", i);
            continue;
        }

        if (port->sig != 0x00000101) {
            TRACE("\tport #%d - %08x", i, port->sig);
            continue;
        }

        TRACE("\tport #%d - SATA", i);

        // create the device
        ahci_interface_t* interface = kalloc(sizeof(ahci_interface_t));
        interface->instance.type = DRIVER_BLOCK;
        interface->instance.meta.refcount = 1;
        interface->instance.meta.dtor = NULL;
        interface->instance.meta.kind = HANDLE_DRIVER;
        interface->instance.block.size = NULL;
        interface->instance.block.read = NULL;
        interface->instance.block.write = NULL;
        interface->port = port;

        // create the thread
        char thread_name[32];
        snprintf(thread_name, sizeof(thread_name), "ahci/%04x:%02x:%02x.%x/%d-handling",
                 data->pci_dev->address.seg, data->pci_dev->address.bus,
                 data->pci_dev->address.slot, data->pci_dev->address.func, i);

        thread_t* thread;
        CHECK_AND_RETHROW(create_thread(&thread, (void*)ahci_thread, interface, thread_name));
        schedule_thread(thread);
        CHECK_AND_RETHROW(release_thread(thread));
    }

cleanup:
    return err;
}

DRIVER {
    .name = "AHCI 1.0",
    .entry = ahci_entry,
    .interface = {
        .type = DRIVER_BLOCK,
    },
    .binds = (driver_bind_t[]) {
        {
            .type = BIND_PCI,
            .pci = {
                .class = 0x01,
                .subclass = 0x06,
                .progif = 0x01
            }
        },
        {BIND_END }
    }
};