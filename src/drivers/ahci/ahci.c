#include <drivers/storage_object.h>
#include <drivers/pci/pci.h>
#include <drivers/pci/pci_spec.h>
#include <memory/vmm.h>
#include <processes/process.h>
#include <stb/stb_ds.h>
#include <util/defs.h>
#include "ahci.h"
#include "sata_spec.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Ahci controller implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static error_t handle_ahci_interrupt(interrupt_context_t* context, void* _self) {
    error_t err = NO_ERROR;
    ahci_controller_t* self = cast(AhciController(), _self);

//    // make sure the device is responsible for the interrupt
//    if(!self->device->header->status.interrupt) {
//        goto not_our_device;
//    }

    for(int i = 0; i < 32; i++) {
        // check if the port is valid and if we got an interrupt on it
        if(self->devices[i] != NULL && self->hba->is & (1u << i)) {
            ahci_device_t* device = self->devices[i];
            if(device->port->is & (AHCI_PxIS_TFES)) {
                debug_log("[-] ahci: TFES: %b", device->fis->rfis.error);
                ASSERT(false);
            }

            // if related to transaction complete
            if(self->hba->is & AHCI_PxIS_DHRS) {
                for (size_t j = 0; j < 32; j++) {
                    // if this is clear then it is done and we should notify the waiting thread
                    if ((device->port->ci & (1u << j)) == 0 && device->slot_in_use[j]) {
                        event_signal(&device->interrupt_on_slot[j]);
                    }
                }
            }
        }
    }


cleanup:
    lapic_send_eoi();

not_our_device:
    return err;
}

static void* AhciController_ctor(void* _self, va_list ap) {
    ahci_controller_t* self = super_ctor(AhciController(), _self, ap);

    self->device = cast(PciDevice(), va_arg(ap, void*));

    // setup the device (enable bus master and enable interrupts)
    self->device->header->command.bus_master_enable = true;
    self->device->header->command.interrupt_disable = false;

    // setup smi if can
    self->interrupt.name = "AHCI Controller";
    self->interrupt.callback = handle_ahci_interrupt;
    self->interrupt.user_param = self;
    self->interrupt.vector = -1;
    interrupt_register(&self->interrupt);
    pci_msi_setup(self->device, self->interrupt.vector);

    // initialize the bar (and map it)
    ASSERT(self->device->bars[5].base != 0);
    self->hba = (AHCI_HBA*) self->device->bars[5].base;
    vmm_map(&kernel_process->vmm_handle, DIRECT_TO_PHYSICAL(self->device->bars[5].base), self->device->bars[5].base, sizeof(AHCI_HBA), PAGE_SUPERVISOR_READWRITE, DEFAULT_CACHE);

    // bios handoff
    if(self->hba->cap2 & AHCI_CAP2_BOH) {
        self->hba->bohc |= AHCI_BOHC_OOS;
        memory_fence();
        while(!(self->hba->bohc & (AHCI_BOHC_OOS | AHCI_BOHC_BOS))) {
            cpu_pause();
        }
    }

    // enable interrupts
    self->hba->ghc |= AHCI_GHC_IE;

    // probe ports
    for(size_t i = 0; i < 32; i++) {
        if(self->hba->pi & (1u << i)) {
            AHCI_HBA_PORT* port = &self->hba->ports[i];
            switch(port->sig) {
                case AHCI_SIGNATURE_SATA:
                    self->devices[i] = new(AhciDevice(), self, port);

                    acquire_lock(&storage_obejcts_lock);
                    arrpush(storage_objects, self->devices[i]);
                    release_lock(&storage_obejcts_lock);
                    break;

                default:
                    break;
            }
        }
    }

    return self;
}

static void* AhciController_dtor(void* _self, va_list ap) {
    ahci_controller_t* self = super_dtor(AhciController(), _self);

    // disable the bus mastering and interrupts of the device
    self->device->header->command.bus_master_enable = false;
    self->device->header->command.interrupt_disable = true;

    // free the interrupt handler
    interrupt_free(&self->interrupt);

    // disable MSI
    if(self->device->msi != NULL) {
        self->device->msi->msi_enable = false;
    }

    // TODO: delete all device objects related to this object

    return _self;
}

const void* AhciController() {
    static const void* class = NULL;
    if(class == NULL) {
        class = new(Class(),
                    "AhciController", Object(), sizeof(ahci_controller_t),
                    ctor, AhciController_ctor,
                    dtor, AhciController_dtor,
                    0
                );
    }
    return class;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Ahci device implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Will search for a free slot that can be used to send
 * a command to the AHCI controller
 *
 * @remark
 * This will automatically lock the slots lock, BUT if a slot is found,
 * the slots lock is stayed locked until released manually by the caller!
 *
 * @param dev   [IN] The device to find a free lock on
 */
static int find_free_slot(ahci_device_t* dev) {
    acquire_lock(&dev->slots_lock);

    uint32_t slots = dev->port->sact | dev->port->ci;
    for(int i = 0; i < 32; i++) {
        if((slots & 1u) == 0 && !dev->slot_in_use[i]) {
            dev->slot_in_use[i] = true;
            release_lock(&dev->slots_lock);
            return i;
        }
        slots >>= 1u;
    }

    release_lock(&dev->slots_lock);
    return -1;
}

/**
 * This is the port initialization function
 *
 * it takes in the controller and port struct as parameters and will make
 * sure to allocate the fis and command lists, identify the disk and size
 * of block, and preallocate a command table [er block
 *
 * @param _self
 * @param ap
 * @return
 */
static void* AhciDevice_ctor(void* _self, va_list ap) {
    ahci_device_t* self = super_ctor(AhciDevice(), _self, ap);

    self->controller = cast(AhciController(), va_arg(ap, ahci_controller_t*));
    self->port = va_arg(ap, AHCI_HBA_PORT*);

    // stop the device
    self->port->cmd &= ~AHCI_PxCMD_ST;
    memory_fence();
    while(self->port->cmd & (AHCI_PxCMD_FR | AHCI_PxCMD_CR)) {
        cpu_pause();
    }
    self->port->cmd &= ~AHCI_PxCMD_FRE;
    memory_fence();

    // allocate memory for the command list and fis
    self->cl = mm_allocate_pages(SIZE_TO_PAGES(sizeof(AHCI_HBA_CMD_HEADER) * 32));
    self->fis = mm_allocate_pages(SIZE_TO_PAGES(sizeof(AHCI_HBA_FIS)));

    // set interrupts we wanna get
    self->port->ie = (AHCI_PxIE_TFEE | AHCI_PxIE_DHRE);

    uintptr_t phys_cl = DIRECT_TO_PHYSICAL((uintptr_t)self->cl);
    self->port->clbu = ((phys_cl >> 32u) & 0xFFFFFFFF);
    self->port->clb = phys_cl & 0xFFFFFFFF;

    uintptr_t phys_fis = DIRECT_TO_PHYSICAL((uintptr_t)self->fis);
    self->port->fbu = ((phys_fis >> 32u) & 0xFFFFFFFF);
    self->port->fb = phys_fis & 0xFFFFFFFF;

    // setup receive fis
    self->fis->dsfis.h.fis_type = FIS_TYPE_DMA_SETUP;
    self->fis->psfis.h.fis_type = FIS_TYPE_PIO_SETUP;
    self->fis->rfis.h.fis_type = FIS_TYPE_REG_D2H;
    self->fis->sdbfis[0] = FIS_TYPE_DEV_BITS;

    // initialize all of the command headers
    // TODO: eventually make this a bit more dynamic or something
    for(int i = 0; i < 32; i++) {
        AHCI_HBA_CMD_HEADER* cmd = &self->cl[i];
        AHCI_HBA_CMD_TBL* cmdtbl = mm_allocate_pages(SIZE_TO_PAGES(offsetof(AHCI_HBA_CMD_TBL, prdt_entry) + sizeof(AHCI_HBA_PRDT_ENTRY)));
        uintptr_t phys_ctba = DIRECT_TO_PHYSICAL((uintptr_t)cmdtbl);
        cmd->ctbau = ((phys_ctba >> 32u) & 0xFFFFFFFF);
        cmd->ctba = phys_ctba & 0xFFFFFFFF;
    }

    // start the device again
    memory_fence();
    while(self->port->cmd & AHCI_PxCMD_CR) {
        cpu_pause();
    }
    self->port->cmd |= (AHCI_PxCMD_FRE | AHCI_PxCMD_ST);
    memory_fence();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // identify
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // TODO: ugly af

    // prepare the command
    int slot = find_free_slot(self);
    ASSERT(slot != -1);
    AHCI_HBA_CMD_HEADER* cmd = &self->cl[slot];
    cmd->cfl = sizeof(FIS_REG_H2D) / sizeof(uint32_t);
    cmd->w = 0;
    cmd->prdtl = 1;

    // allocate the data and command table
    ATA_IDENTIFY_DATA* identify = mm_allocate_pages(SIZE_TO_PAGES(sizeof(ATA_IDENTIFY_DATA)));
    AHCI_HBA_CMD_TBL* cmdtbl = (AHCI_HBA_CMD_TBL*)PHYSICAL_TO_DIRECT(cmd->ctba | ((uintptr_t)cmd->ctbau << 32u));

    // set the command entry
    uintptr_t phys_buf = DIRECT_TO_PHYSICAL((uintptr_t)identify);
    cmdtbl->prdt_entry[0].dba = phys_buf & 0xFFFFFFFF;
    cmdtbl->prdt_entry[0].dbau = ((phys_buf >> 32u) & 0xFFFFFFFF);
    cmdtbl->prdt_entry[0].dbc = sizeof(ATA_IDENTIFY_DATA) - 1;
    cmdtbl->prdt_entry[0].i = 1;

    // setup the fis
    FIS_REG_H2D* fis = (FIS_REG_H2D*)&cmdtbl->cfis;
    memset(fis, 0, sizeof(FIS_REG_H2D));
    fis->h.fis_type = FIS_TYPE_REG_H2D;
    fis->command = ATA_CMD_IDENTIFY;
    fis->h.c = 1;

    // wait for the device to not be busy
    memory_fence();
    while(self->port->tfd & (AHCI_PxTFD_STS_BSY | AHCI_PxTFD_STS_DRQ)) {
        cpu_pause();
    }

    // Issue command
    memory_fence();
    self->port->ci = 1u << slot;

    // wait for completion
    while(true) {
        if((self->port->ci & (1u << slot)) == 0) {
            break;
        }else if(self->port->is & AHCI_PxIS_TFES) {
            ASSERT(false);
        }
        cpu_pause();
    }

    // prepare the model name
    char buffer[20 + 1] = {0};
    for(int i = 0; i < 10; i++) {
        buffer[i * 2 + 0] = (char)((identify->model_number[i] >> 8u) & 0xFFu);
        buffer[i * 2 + 1] = (char)(identify->model_number[i] & 0xFFu);
    }

    // now do stuff with the identify data
    debug_log("[*] ahci: \tfound: %s\n", buffer);

    //figure the sector size
    self->block_size = 512;
    if((identify->physical_logical_sector & (1u << 12u)) && identify->logical_sector_size[0] != 0) {
        self->block_size = identify->logical_sector_size[0] << 1;
    }

    // free the buffers
    mm_free_pages((void*)identify, SIZE_TO_PAGES(sizeof(ATA_IDENTIFY_DATA)));
    mm_free_pages(cmdtbl, SIZE_TO_PAGES(offsetof(AHCI_HBA_CMD_TBL, prdt_entry) + sizeof(cmd->prdtl)));

    // make sure to set that this is not in use
    self->slot_in_use[0] = false;

    // now that we know the bytes per block, we can setup everything nicely
    // so we have preallocated buffers
    for(int i = 0; i < 32; i++) {
        AHCI_HBA_CMD_HEADER* cmd = &self->cl[i];
        AHCI_HBA_CMD_TBL* cmdtbl = (AHCI_HBA_CMD_TBL*)PHYSICAL_TO_DIRECT(cmd->ctba | ((uintptr_t)cmd->ctbau << 32u));
        void* max_data_buffer = mm_allocate_pages(SIZE_TO_PAGES(self->block_size));
        uintptr_t phys_buf = DIRECT_TO_PHYSICAL((uintptr_t)max_data_buffer);
        cmdtbl->prdt_entry[0].dba = phys_buf & 0xFFFFFFFF;
        cmdtbl->prdt_entry[0].dbau = ((phys_buf >> 32u) & 0xFFFFFFFF);
        cmdtbl->prdt_entry[0].dbc = self->block_size - 1;
        cmdtbl->prdt_entry[0].i = 1;

    }

    return _self;
}

static void* AhciDevice_dtor(void* _self) {
    __attribute__((unused)) ahci_device_t* self = super_dtor(AhciDevice(), _self);
    // TODO: ahci dtor
    ASSERT(false);
    return _self;
}

static size_t AhciDevice_get_block_size(void* _self) {
    ahci_device_t* device = cast(AhciDevice(), _self);
    return device->block_size;
}

/**
 * This is the block read function
 *
 * it takes in the block address and returns reads the block into the buffer
 *
 * @remark
 * Right now this is not the most optimized method to do this... would be better if you could
 * request a range of blocks or something and it would handle that for you, because block at a time
 * might be too slow...
 */
static error_t AhciDevice_read_block(void* _self, uintptr_t lba, void* buffer) {
    error_t err = NO_ERROR;
    ahci_device_t* self = cast(AhciDevice(), _self);

    // wait to get a free slot
    int slot = find_free_slot(self);
    while(slot == -1) {
        event_wait(&self->has_empty_slot);
        slot = find_free_slot(self);
    }

    // get the command
    AHCI_HBA_CMD_HEADER* cmd = &self->cl[slot];
    cmd->cfl = sizeof(FIS_REG_H2D) / sizeof(uint32_t);
    cmd->w = 0;
    cmd->prdtl = 1;

    // get the buffer that the result will be written to
    AHCI_HBA_CMD_TBL* cmdtbl = (AHCI_HBA_CMD_TBL*)PHYSICAL_TO_DIRECT(cmd->ctba | ((uintptr_t)cmd->ctbau << 32u));
    void* buf = (void*)PHYSICAL_TO_DIRECT(cmdtbl->prdt_entry[0].dba | ((uint64_t)cmdtbl->prdt_entry[0].dbau) << 32u);

    // setup the fis
    FIS_REG_H2D* fis = (FIS_REG_H2D*)&cmdtbl->cfis;
    memset(fis, 0, sizeof(FIS_REG_H2D));
    fis->h.fis_type = FIS_TYPE_REG_H2D;
    fis->command = ATA_CMD_READ_DMA_EXT;
    fis->device = (1 << 6); // lba mode
    fis->h.c = 1;
    fis->lba0 = lba & 0xFFu;
    fis->lba1 = ((lba >> 8u) & 0xFFu);
    fis->lba2 = ((lba >> 16u) & 0xFFu);
    fis->lba3 = ((lba >> 24u) & 0xFFu);
    fis->lba4 = ((lba >> 32u) & 0xFFu);
    fis->lba5 = ((lba >> 40u) & 0xFFu);
    fis->countl = 1;
    fis->counth = 0;

    // wait till not busy to issue the command
    memory_fence();
    while(self->port->tfd & (AHCI_PxTFD_STS_BSY | AHCI_PxTFD_STS_DRQ)) {
        cpu_pause();
    }

    // issue the command
    self->port->ci |= 1u << slot;

    // wait for the interrupt and then copy the buffer
    event_wait(&self->interrupt_on_slot[slot]);
    memcpy(buffer, buf, self->block_size);

    // tell that we finished using the slot
    self->slot_in_use[slot] = false;
    event_signal(&self->has_empty_slot);

cleanup:
    return err;
}

const void* AhciDevice() {
    static const void* class = NULL;
    if(class == NULL) {
        class = new(StorageDeviceClass(),
                    "AhciDevice", Object(), sizeof(ahci_device_t),
                    ctor, AhciDevice_ctor,
                    dtor, AhciDevice_dtor,
                    storage_read_block, AhciDevice_read_block,
                    storage_get_block_size, AhciDevice_get_block_size,
                0);
    }
    return class;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Pci scan
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static pci_sig_t supported_devices[] = {
        { 0x8086, 0x2922 },
        { 0x8086, 0x2829 }
};

static const char* supported_devices_names[] = {
        "Intel 82801IR/IO/IH (ICH9R/DO/DH) 6 port SATA Controller [AHCI mode]",
        "Intel 82801HM/HEM (ICH8M/ICH8M-E) SATA Controller [AHCI mode]",
};

void ahci_scan() {
    for(list_entry_t* link = pci_devices.next; link != &pci_devices; link = link->next) {
        pci_device_t* pci = CR(link, pci_device_t, link);

        // start by looking at devices that we support by vendor and device ids
        bool supported = false;
        for(int i = 0; i < ARRAY_LEN(supported_devices); i++) {
            if(pci->header->device_id == supported_devices[i].device_id && pci->header->vendor_id == supported_devices[i].vendor_id) {
                debug_log("[*] ahci: %s found\n", supported_devices_names[i]);
                supported = true;
                break;
            }
        }

        // if we don't know the ids, at least check the class code, and initialize it anyways
        if(!supported) {
            if(pci->header->class_code == 0x01 && pci->header->subclass_code == 0x06 && pci->header->program_interface == 0x01) {
                debug_log("[!] ahci: Unknown device found (%04x:%04x), initializing anyways!\n", pci->header->vendor_id, pci->header->device_id);
                supported = true;
            }
        }

        // if the device is supported intialize the controller
        // it will make sure to initialize all the children
        if(supported) {
            new(AhciController(), pci);
        }
    }
}
