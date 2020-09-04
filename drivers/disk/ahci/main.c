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

static err_t do_read_write(driver_instance_t* instance, size_t lba, void* buffer, size_t buffer_size, bool write) {
    err_t err = NO_ERROR;
    ahci_interface_t* interface = CR(instance, ahci_interface_t, instance);
    hba_cmd_table_t* cmdtbl = NULL;
    vbuffer_t* vb = NULL;

    // make sure everything is aligned nicely
    CHECK(buffer_size % instance->block.block_size == 0);
    CHECK(lba + (buffer_size / instance->block.block_size) <= instance->block.last_block);

    // setup the request
    ahci_request_t request;
    CHECK_AND_RETHROW(create_event(&request.event));

    // setup the header
    request.header.cfl = sizeof(fis_reg_h2d_t) / sizeof(uint32_t);
    request.header.prdtl = 1;
    request.header.w = write;

    // calculate the number of entries needed
    int prdt_entries = ALIGN_UP(buffer_size, SIZE_4MB) / SIZE_4MB;

    // get vbuffer of the memory
    CHECK_AND_RETHROW(vmm_get_vbuffer(&g_current_process->address_space, buffer, buffer_size, &vb));

    // setup the command table
    cmdtbl = pmallocz(sizeof(hba_cmd_table_t) + sizeof(hba_prdt_entry_t) * prdt_entries);
    request.header.ctba = DIRECT_TO_PHYSICAL(cmdtbl);
    for (int i = 0; i < prdt_entries; i++) {
        cmdtbl->prdts[i].dba = DIRECT_TO_PHYSICAL(vb->pages[i * (SIZE_4MB / PAGE_SIZE)]) + vb->offset;
        cmdtbl->prdts[i].dbc = MIN(SIZE_4MB, vb->size);
        cmdtbl->prdts[i].i = 1;

        vb->offset = 0;
        vb->size -= MIN(SIZE_4MB, vb->size);
    }

    // setup the fis
    fis_reg_h2d_t* fis = (fis_reg_h2d_t*)cmdtbl->cfis;
    fis->fis_type = FIS_TYPE_REG_H2D;
    fis->command = write ? ATA_CMD_WRITE_DMA_EXT : ATA_CMD_READ_DMA_EXT;
    fis->device = BIT6;
    fis->c = 1;
    fis->lba0 = (lba & 0xFFu);
    fis->lba1 = ((lba >> 8u) & 0xFFu);
    fis->lba2 = ((lba >> 16u) & 0xFFu);
    fis->lba3 = ((lba >> 24u) & 0xFFu);
    fis->lba4 = ((lba >> 32u) & 0xFFu);
    fis->lba5 = ((lba >> 40u) & 0xFFu);
    fis->count = buffer_size / instance->block.block_size;

    // queue it and signal that there is a request
    ticket_lock(&interface->request_lock);
    list_add(&interface->request_queue, &request.link);
    ticket_unlock(&interface->request_lock);
    CHECK_AND_RETHROW(signal_event(interface->request_event));

    // wait for it
    CHECK_AND_RETHROW(wait_for_event(&request.event, 1, NULL));

cleanup:
    close_event(request.event);
    if (cmdtbl) {
        pmfree(cmdtbl, sizeof(hba_cmd_table_t) + sizeof(hba_prdt_entry_t) * prdt_entries);
    }
    if (vb) {
        release_vbuffer(vb);
    }
    return err;
}

static err_t do_read(driver_instance_t* instance, size_t lba, void* buffer, size_t buffer_size) {
    return do_read_write(instance, lba, buffer, buffer_size, false);
}

static err_t do_write(driver_instance_t* instance, size_t lba, void* buffer, size_t buffer_size) {
    return do_read_write(instance, lba, buffer, buffer_size, true);
}

/**
 * Will request an identify from the device and setup the device
 * information
 */
static err_t request_identify(ahci_interface_t* interface) {
    err_t err = NO_ERROR;
    hba_cmd_table_t* cmdtbl = NULL;

    // setup the request
    ahci_request_t request = {0};
    volatile ata_identify_data_t* identify_data = pmallocz(sizeof(ata_identify_data_t));
    CHECK_AND_RETHROW(create_event(&request.event));

    // setup the header
    request.header.cfl = sizeof(fis_reg_h2d_t) / sizeof(uint32_t);
    request.header.prdtl = 1;

    // setup the command table
    cmdtbl = pmallocz(sizeof(hba_cmd_table_t) + sizeof(hba_prdt_entry_t));
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

    // resolve the capacity of the device
    size_t capacity;
    if (identify_data->command_set_supported_83 & BIT10) {
        // 48bit addressing support
        capacity = 0;
        for (int i = 0; i < 4; i++) {
            capacity |= (size_t)identify_data->maximum_lba_for_48bit_addressing[i] << (16ull * i);
        }
    } else {
        capacity = (identify_data->user_addressable_sectors_hi << 16) | identify_data->user_addressable_sectors_lo;
    }
    interface->instance.block.last_block = capacity - 1;

    // resolve the block size
    size_t block_size = 512;
    if ((identify_data->phy_logic_sector_support & (BIT14 | BIT15)) == BIT14) {
        if ((identify_data->phy_logic_sector_support & BIT12) != 0) {
            block_size = ((identify_data->logic_sector_size_hi << 16) | identify_data->logic_sector_size_lo) * sizeof(uint16_t);
        }
    }
    interface->instance.block.block_size = block_size;

    // set the name
    for (int i = 0; i < 40; i += 2) {
        interface->instance.name[i] = identify_data->model_name[i + 1];
        interface->instance.name[i + 1] = identify_data->model_name[i];
    }

    interface->instance.name[40] = '\0';
    for (int i = 40 - 1; i >= 0; i--) {
        if (interface->instance.name[i] != ' ') {
            break;
        }
        interface->instance.name[i] = '\0';
    }



cleanup:
    // cleanup everything
    if (request.event) {
        close_event(request.event);
    }
    if (cmdtbl) {
        pmfree(cmdtbl, sizeof(hba_cmd_table_t) + sizeof(hba_prdt_entry_t));
    }
    if (identify_data) {
        pmfree((directptr_t)identify_data, sizeof(*identify_data));
    }
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
        interface->instance.block.read_blocks = do_read;
        interface->instance.block.write_blocks = do_write;

        // request stuff
        interface->request_queue = INIT_LIST(interface->request_queue);
        CHECK_AND_RETHROW(create_event(&interface->request_event));

        // cmd stuff
        interface->cmd_list_len = cap.ncs + 1;
        interface->s64a = cap.s64a;
        interface->port = port;

        // create and start the thread for the port
        thread_t* port_thread;
        CHECK_AND_RETHROW(create_thread(&port_thread, (void *) ahci_thread, interface,
                                        "ahci/%04x:%02x:%02x.%x/p%d",
                                        data->pci_dev->address.seg, data->pci_dev->address.bus,
                                        data->pci_dev->address.slot, data->pci_dev->address.func, i));
        schedule_thread(port_thread);
        CHECK_AND_RETHROW(release_thread(port_thread));

        // setup everything that is left to handle
        CHECK_AND_RETHROW(request_identify(interface));

        // register the interface
        CHECK_AND_RETHROW(register_interface(&interface->instance));

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