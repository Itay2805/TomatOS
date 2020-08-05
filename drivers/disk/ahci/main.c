#include <driver/pci/pci_spec.h>
#include <driver/driver.h>
#include <proc/process.h>
#include <util/defs.h>
#include <proc/scheduler.h>
#include <mem/mm.h>
#include <mem/pmm.h>
#include "ahci_spec.h"

#undef __MODULE__
#define __MODULE__ "ahci"

typedef struct ahci_interface {
    driver_instance_t instance;
    volatile hba_port_t *port;
    int cmd_list_len;
    hba_cmd_header_t *cmd_list;
} ahci_interface_t;

typedef struct ahci_controller {
    volatile hba_mem_t *hba;
    uint8_t vector;
    ahci_interface_t *interfaces[32];
} ahci_controller_t;

/**
 * check if the device fired and the irq thread should wakeup
 */
bool ahci_wakeup(ahci_controller_t *controller) {
    return controller->hba->is;
}

static void ahci_interrupt(ahci_controller_t *controller) {
    TRACE("Started %s", g_current_thread->name);

    while (true) {
        wait_for_interrupt(controller->vector, (interrupt_wakeup_t) ahci_wakeup, controller);

        uint32_t is = controller->hba->is;
        for (size_t i = next_bit(is, 0); i < 64; i = next_bit(is, i + 1)) {
            TRACE("port %d fired interrupt", i);
        }
        controller->hba->is = is;
    }

    ASSERT_TRACE(false, "Tried to exit `%s`", g_current_thread->name);
}

/**
 * AHCI port handling thread, will handle initialization and requests to
 * the given port.
 */
static void ahci_thread(ahci_interface_t *interface) {
    err_t err = NO_ERROR;
    volatile hba_port_t *port = interface->port;

    TRACE("initializing %s", g_current_thread->name);

    // TODO setup PxIE

    // make sure to stop the cmd before doing anything
    port->cmd &= ~HBA_PxCMD_ST;
    port->cmd &= ~HBA_PxCMD_FRE;
    while (1) {
        if (!(port->cmd & HBA_PxCMD_FR) && !(port->cmd & HBA_PxCMD_CR)) {
            break;
        }
        cpu_pause();
    }

    // set the command list and fis
    // TODO: support 32bit only
    interface->cmd_list = pmallocz(SIZE_1KB);
    interface->port->clb = DIRECT_TO_PHYSICAL(interface->cmd_list);
    interface->port->fb = DIRECT_TO_PHYSICAL(pmallocz(256));

    // set all entries to be zero
    for (int i = 0; i < interface->cmd_list_len; i++) {
        interface->cmd_list[i].prdtl = 0;
        interface->cmd_list[i].ctba = 0;
    }

    // start the command engine
    while (port->cmd & HBA_PxCMD_CR)
        cpu_pause();
    port->cmd |= HBA_PxCMD_FRE;
    port->cmd |= HBA_PxCMD_ST;

    // register the interface
    CHECK_AND_RETHROW(register_interface((void *) interface));

    // we are ready to accept requests
    TRACE("%s ready", g_current_thread->name);
    while (1) {
        yield();
    }

    cleanup:
    (void) err;
}

/**
 * Driver entry, will setup the controller, initalize the ports, and start handling threads
 * for each of the ports.
 */
static err_t ahci_entry(driver_bind_data_t *data) {
    err_t err = NO_ERROR;

    // check the device
    CHECK(data != NULL);
    CHECK(data->bind->type == BIND_PCI);

    // sanity check the bar
    CHECK(data->pci_dev->bars[5].present);
    CHECK(data->pci_dev->bars[5].mmio);
    hba_mem_t* hba = data->pci_dev->bars[5].addr;

    // make sure we have irq/msi/msix support
    CHECK(data->pci_dev->irq != -1);

    // get the hba
    ahci_controller_t *controller = kalloc(sizeof(ahci_controller_t));
    controller->hba = hba;

    // gain ownership over the device
    if (hba->cap2 & HBA_CAP2_BOH && hba->bohc & HBA_BOHC_BOS) {
        hba->bohc |= HBA_BOHC_OOS;
        ustall(1);

        while (hba->bohc & HBA_BOHC_BB) {
            cpu_pause();
        }

        CHECK(hba->bohc & HBA_BOHC_OOS);
    }

    // make sure interrupts are enabled and that we are in ahci mode
    hba->ghc |= HBA_GHC_IE | HBA_GHC_AE;

    // now check the caps
    hba_cap_t cap = hba->cap;
    CHECK_TRACE(cap.s64a, "TODO: add support for non-64bit addressing");

    // iterate ports
    uint32_t pi = hba->pi;
    for (int i = 0; i < 32; i++) {
        if (!(pi & (1u << i))) continue;

        volatile hba_port_t *port = &controller->hba->ports[i];
        if (port->det != 3 || port->ipm != 1) {
            continue;
        }

        if (port->sig != 0x00000101) {
            continue;
        }

        // create the device
        ahci_interface_t *interface = kalloc(sizeof(ahci_interface_t));
        interface->instance.type = DRIVER_BLOCK;
        interface->instance.meta.refcount = 1;
        interface->instance.meta.dtor = NULL;
        interface->instance.meta.kind = HANDLE_DRIVER;
        interface->instance.block.size = NULL;
        interface->instance.block.read = NULL;
        interface->instance.block.write = NULL;
        interface->cmd_list_len = cap.ncs + 1;
        interface->port = port;

        // create the thread
        char thread_name[32];
        snprintf(thread_name, sizeof(thread_name), "ahci/%04x:%02x:%02x.%x/p%d",
                 data->pci_dev->address.seg, data->pci_dev->address.bus,
                 data->pci_dev->address.slot, data->pci_dev->address.func, i);

        thread_t *thread;
        CHECK_AND_RETHROW(create_thread(&thread, (void *) ahci_thread, interface, thread_name));
        schedule_thread(thread);
        CHECK_AND_RETHROW(release_thread(thread));
    }

    // TODO: msi/msix
    CHECK_AND_RETHROW(register_irq(data->pci_dev->irq, &controller->vector));

    char thread_name[32];
    snprintf(thread_name, sizeof(thread_name), "ahci/%04x:%02x:%02x.%x/irq",
             data->pci_dev->address.seg, data->pci_dev->address.bus,
             data->pci_dev->address.slot, data->pci_dev->address.func);

    thread_t *thread;
    CHECK_AND_RETHROW(create_thread(&thread, (void *) ahci_interrupt, controller, thread_name));
    schedule_thread(thread);
    CHECK_AND_RETHROW(release_thread(thread));

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
                {BIND_END}
        }
};