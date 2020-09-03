#include <driver/pci/pci_spec.h>
#include <driver/driver.h>
#include <proc/process.h>
#include <util/defs.h>
#include <proc/scheduler.h>
#include <mem/mm.h>
#include <mem/pmm.h>
#include <proc/event.h>
#include "ahci_spec.h"
#include "../ata_spec.h"

#undef __MODULE__
#define __MODULE__ "ahci"

typedef struct ahci_interface {
    // the interface stuff
    driver_instance_t instance;
    volatile hba_port_t* port;
    event_t port_event;

    // command manipulation
    int cmd_list_len;
    hba_cmd_header_t* cmd_list;
    uint32_t free_cmds;
    uint32_t used_cmds;
    struct ahci_request* requests[32];
    ticket_lock_t cmd_lock;

    // the queue of commands to the device
    list_entry_t request_queue;
    ticket_lock_t request_lock;
    event_t request_event;

    // misc
    bool s64a;
} ahci_interface_t;

typedef struct ahci_controller {
    volatile hba_mem_t* hba;
    uint8_t vector;
    event_t event;
    ahci_interface_t* interfaces[32];
} ahci_controller_t;

typedef struct ahci_request {
    // the event to trigger on completion
    event_t event;

    // the header to add
    hba_cmd_header_t header;

    // the link in the request queue
    list_entry_t link;
} ahci_request_t;

/**
 * check if the device fired and the irq thread should wakeup
 */
err_t ahci_irq(ahci_controller_t* controller) {
    err_t err = NO_ERROR;

    if (controller->hba->is) {
        CHECK_AND_RETHROW(signal_event(controller->event));
    }

cleanup:
    return NO_ERROR;
}

static void ahci_interrupt(ahci_controller_t *controller) {
    err_t err = NO_ERROR;
    TRACE("Started %s", g_current_thread->name);

    while (true) {
        CHECK_AND_RETHROW(wait_for_event(&controller->event, 1, NULL));

        uint32_t is = controller->hba->is;
        for (size_t i = next_bit(is, 0); i < 64; i = next_bit(is, i + 1)) {
            CHECK_AND_RETHROW(signal_event(controller->interfaces[i]->port_event));
        }
        controller->hba->is = is;
    }

cleanup:
    ASSERT_TRACE(false, "Tried to exit `%s` - %s", g_current_thread->name, strerror(err));
}

/**
 * AHCI port handling thread, will handle initialization and requests to
 * the given port.
 */
static void ahci_thread(ahci_interface_t *interface) {
    err_t err = NO_ERROR;
    volatile hba_port_t *port = interface->port;

    TRACE("initializing %s", g_current_thread->name);

    // cmd stuff
    interface->free_cmds = (1ull << interface->cmd_list_len) - 1;
    CHECK_AND_RETHROW(create_event(&interface->port_event));

    // setup the interrupts
    port->ie = PxIE_DHRE;

    // make sure to stop the cmd before doing anything
    port->cmd &= ~PxCMD_ST;
    port->cmd &= ~PxCMD_FRE;
    while (1) {
        if (!(port->cmd & PxCMD_FR) && !(port->cmd & PxCMD_CR)) {
            break;
        }
        cpu_pause();
    }

    // set the command list and fis
    interface->cmd_list = interface->s64a ? pmallocz(SIZE_1KB) : pmallocz_low(SIZE_1KB);
    interface->port->clb = DIRECT_TO_PHYSICAL(interface->cmd_list);
    interface->port->fb = DIRECT_TO_PHYSICAL(interface->s64a ? pmallocz(256) : pmallocz_low(256));

    // start the command engine
    while (port->cmd & PxCMD_CR)
        cpu_pause();
    port->cmd |= PxCMD_FRE;
    port->cmd |= PxCMD_ST;

    // register the interface
    CHECK_AND_RETHROW(register_interface((void *) interface));

    // we are ready to accept requests
    TRACE("%s ready", g_current_thread->name);
    event_t events[] = { interface->port_event, interface->request_event };
    while (1) {
        size_t index;
        CHECK_AND_RETHROW(wait_for_event(events, ARRAY_LENGTH(events), &index));

        // got a port event
        if (index == 0) {
            if (port->is & PxIS_DHRS) {
                // transaction complete
                for (size_t slot = next_bit(interface->used_cmds, 0); slot < interface->cmd_list_len; slot = next_bit(interface->used_cmds, slot + 1)) {
                    if (!(port->ci & slot)) {
                        // check the request
                        ahci_request_t* request = interface->requests[slot];
                        CHECK_AND_RETHROW(signal_event(request->event));

                        // can free the slot
                        interface->used_cmds &= ~slot;
                        interface->free_cmds |= slot;
                    }
                }
            }

            // TODO: errors and such
        }

        // always check if there are requests to handle
        size_t slot = 0;
        ticket_lock(&interface->request_lock);
        while (!list_is_empty(&interface->request_queue)) {
            slot = next_bit(interface->free_cmds, slot + 1);
            if (slot >= interface->cmd_list_len) {
                // no commands available
                break;
            }

            interface->free_cmds &= ~(1u << slot);
            interface->used_cmds |= (1u << slot);

            // dequeue
            ahci_request_t* request = CR(interface->request_queue.next, ahci_request_t, link);
            list_del(&request->link);

            // set the command and fire it
            interface->requests[slot] = request;
            interface->cmd_list[slot] = request->header;
            port->ci = 1 << slot;
        }
        ticket_unlock(&interface->request_lock);
    }

cleanup:
    ASSERT(!IS_ERROR(err));
}

/**
 * Will request an identify from the device and setup the device
 * information
 */
static err_t request_identify(ahci_interface_t* interface) {
    err_t err = NO_ERROR;

    // setup the request
    ahci_request_t request;
    volatile ata_identify_data_t* identify_data = pmallocz(sizeof(ata_identify_data_t));
    CHECK_AND_RETHROW(create_event(&request.event));

    // setup the header
    request.header.cfl = sizeof(fis_reg_h2d_t) / sizeof(uint32_t);
    request.header.prdtl = 1;

    // setup the command table
    hba_cmd_table_t* cmdtbl = pmallocz(sizeof(hba_cmd_table_t) + sizeof(hba_prdt_entry_t));
    request.header.ctba = DIRECT_TO_PHYSICAL(cmdtbl);
    cmdtbl->prdts[0].dba = DIRECT_TO_PHYSICAL(identify_data);
    cmdtbl->prdts[0].dbc = sizeof(*identify_data) - 1;
    cmdtbl->prdts[0].i = 1;

    // setup the fis
    fis_reg_h2d_t* fis = (fis_reg_h2d_t*)cmdtbl->cfis;
    fis->fis_type = FIS_TYPE_REG_H2D;
    fis->command = ATA_CMD_IDENTIFY;
    fis->c = 1;

    // queue it and signal that there is a request
    ticket_lock(&interface->request_lock);
    list_add(&interface->request_queue, &request.link);
    ticket_unlock(&interface->request_lock);
    CHECK_AND_RETHROW(signal_event(interface->request_event));

    // wait for it
    CHECK_AND_RETHROW(wait_for_event(&request.event, 1, NULL));

    // get the name
    char model_number[20];
    for(int i = 0; i < 10; i++) {
        model_number[i * 2 + 0] = (char)((identify_data->model_number[i] >> 8u) & 0xFFu);
        model_number[i * 2 + 1] = (char)(identify_data->model_number[i] & 0xFFu);
    }
    TRACE("Model number %s", model_number);

cleanup:
    return err;
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
    CHECK_AND_RETHROW(create_event(&controller->event));

    // gain ownership over the device
    if (hba->cap2 & CAP2_BOH) {
        hba->bohc |= BOHC_OOS;
        ustall(1);

        uint64_t started = uptime();
        bool is_busy = false;
        while (hba->bohc & BOHC_BOS) {

            // see if BIOS Busy has been set
            if (!is_busy) {
                if (!(hba->bohc & BOHC_BB)) {
                    if (uptime() - started > 25000) {
                        // BIOS Busy was not set within 25ms, just continue
                        break;
                    }
                } else {
                    is_busy = true;
                }
            } else {
                CHECK_TRACE(uptime() - started < 2000000, "Failed to gain ownership from BIOS within 2 seconds");
            }

            cpu_pause();
        }
        CHECK(hba->bohc & BOHC_OOS);
    }

    // make sure interrupts are enabled and that we are in ahci mode
    hba->ghc |= GHC_IE | GHC_AE;

    // now check the caps
    hba_cap_t cap = hba->cap;

    // setup the irq handling thread
    // TODO: msi/msix
    CHECK_AND_RETHROW(register_irq(data->pci_dev->irq, &controller->vector));
    CHECK_AND_RETHROW(register_irq_handler(controller->vector, (void*)ahci_irq, controller));

    thread_t* irq_thread;
    CHECK_AND_RETHROW(create_thread(&irq_thread, (void *) ahci_interrupt, controller,
                                    "ahci/%04x:%02x:%02x.%x/irq",
                                    data->pci_dev->address.seg, data->pci_dev->address.bus,
                                    data->pci_dev->address.slot, data->pci_dev->address.func));
    schedule_thread(irq_thread);
    CHECK_AND_RETHROW(release_thread(irq_thread));

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
        controller->interfaces[i] = interface;

        // interface
        interface->instance.type = DRIVER_BLOCK;
        interface->instance.meta.refcount = 1;
        interface->instance.meta.dtor = NULL;
        interface->instance.meta.kind = HANDLE_DRIVER;
        interface->instance.block.size = NULL;
        interface->instance.block.read = NULL;
        interface->instance.block.write = NULL;

        // request stuff
        interface->request_queue = INIT_LIST(interface->request_queue);
        CHECK_AND_RETHROW(create_event(&interface->request_event));

        // cmd stuff
        interface->cmd_list_len = cap.ncs + 1;
        interface->s64a = cap.s64a;
        interface->port = port;

        // create and start the threads
        thread_t* port_thread;
        CHECK_AND_RETHROW(create_thread(&port_thread, (void *) ahci_thread, interface,
                                        "ahci/%04x:%02x:%02x.%x/p%d",
                                        data->pci_dev->address.seg, data->pci_dev->address.bus,
                                        data->pci_dev->address.slot, data->pci_dev->address.func, i));
        schedule_thread(port_thread);
        CHECK_AND_RETHROW(release_thread(port_thread));

        CHECK_AND_RETHROW(request_identify(interface));
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
        {BIND_END}
    }
};