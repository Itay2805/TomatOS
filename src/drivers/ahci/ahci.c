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

    // make sure the device is responsible for the interrupt
    if(!self->device->header->status.interrupt) {
        goto not_our_device;
    }

cleanup:
    lapic_send_eoi();

not_our_device:
    return err;
}

static void* AhciController_ctor(void* _self, va_list ap) {
    ahci_controller_t* self = super_ctor(AhciController(), _self, ap);

    self->device = cast(PciDevice(), va_arg(ap, void*));

    // setup the device (enable bus master and disable interrupts)
    self->device->header->command.bus_master_enable = true;
    self->device->header->command.interrupt_disable = true; // TODO: enable interrupts

    // setup smi if can
    self->interrupt.name = "AHCI Controller";
    self->interrupt.callback = handle_ahci_interrupt;
    self->interrupt.user_param = self;
    self->interrupt.vector = -1;
    interrupt_register(&self->interrupt);

    // TODO: setup smi

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

    // probe ports
    for(size_t i = 0; i < 32; i++) {
        if(self->hba->pi & (1u << i)) {
            AHCI_HBA_PORT* port = &self->hba->ports[i];
            switch(port->sig) {
                case AHCI_SIGNATURE_SATA:
                    new(AhciDevice(), self, port);
                    break;

                    // unconnected
                case AHCI_SIGNATURE_DEFAULT:
                    break;

                case AHCI_SIGNATURE_PORT_MULTIPLIER:
                    debug_log("[+] ahci: \t%d: Port multiplier (unsupported)\n", i);
                    break;

                case AHCI_SIGNATURE_ENCLOSURE_MANAGEMENT_BRIDGE:
                    debug_log("[+] ahci: \t%d: enclosure management bridge (unsupported)\n", i);
                    break;

                case AHCI_SIGNATURE_SATAPI:
                    debug_log("[+] ahci: \t%d: SATAPI (unsupported)\n", i);
                    break;

                default:
                    debug_log("[+] ahci: \t%d: Unknown\n", i);
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

    // TODO: disable smi

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

static size_t find_free_slot(ahci_device_t* dev) {
    uint32_t slots = dev->port->sact | dev->port->ci;
    for(int i = 0; i < 32; i++) {
        if((slots & 1u) == 0) {
            return i;
        }
        slots >>= 1u;
    }

    return NULL;
}

static void* AhciDevice_ctor(void* _self, va_list ap) {
    ahci_device_t* self = super_ctor(AhciDevice(), _self, ap);

    self->controller = cast(AhciController(), va_arg(ap, ahci_controller_t*));
    self->port = va_arg(ap, AHCI_HBA_PORT*);

    self->port->is = -1;

    // allocate memory for the command list and fis
    self->cl = mm_allocate_pages(1);
    self->fis = mm_allocate_pages(1);

    uintptr_t phys_cl = DIRECT_TO_PHYSICAL((uintptr_t)self->cl);
    self->port->clbu = ((phys_cl >> 32u) & 0xFFFFFFFF);
    self->port->clb = phys_cl & 0xFFFFFFFF;

    uintptr_t phys_fis = DIRECT_TO_PHYSICAL((uintptr_t)self->fis);
    self->port->fbu = ((phys_fis >> 32u) & 0xFFFFFFFF);
    self->port->fb = phys_fis & 0xFFFFFFFF;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // identify
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // TODO: move to another function
    // TODO: ugly af

    // prepare the command
    size_t slot = find_free_slot(self);
    volatile AHCI_HBA_CMD_HEADER* cmd = &self->cl[slot];
    ASSERT(cmd != NULL);
    cmd->command = sizeof(FIS_REG_H2D) / sizeof(uint32_t);
    cmd->prdtl = 1;

    // find command list slot
    volatile ATA_IDENTIFY_DATA* identify = mm_allocate_pages(SIZE_TO_PAGES(sizeof(ATA_IDENTIFY_DATA)));
    HBA_CMD_TBL* cmdtbl = mm_allocate_pages(SIZE_TO_PAGES(offsetof(HBA_CMD_TBL, prdt_entry) + sizeof(cmd->prdtl)));

    uintptr_t phys_ctba = DIRECT_TO_PHYSICAL((uintptr_t)cmdtbl);
    cmd->ctbau = ((phys_ctba >> 32u) & 0xFFFFFFFF);
    cmd->ctba = phys_ctba & 0xFFFFFFFF;

    uintptr_t phys_buf = DIRECT_TO_PHYSICAL((uintptr_t)cmdtbl);
    cmdtbl->prdt_entry[0].dbau = ((phys_buf >> 32u) & 0xFFFFFFFF);
    cmdtbl->prdt_entry[0].dba = phys_buf & 0xFFFFFFFF;
    cmdtbl->prdt_entry[0].info = sizeof(ATA_IDENTIFY_DATA) - 1;

    // setup the command
    FIS_REG_H2D* fis = (FIS_REG_H2D*)self->fis;
    memset(fis, 0, sizeof(FIS_REG_H2D));
    fis->h.fis_type = FIS_TYPE_REG_H2D;
    fis->command = ATA_CMD_IDENTIFY;
    fis->device = 0;
    fis->h.c = true;

    // wait for the device to not be busy
    memory_fence();
    while(self->port->tfd & (AHCI_PxTFD_STS_BSY | AHCI_PxTFD_STS_DRQ)) {
        cpu_pause();
    }

    // Issue command
    memory_fence();
    self->port->cmd |= ((1u << AHCI_PxCMD_FRE) | (1u << AHCI_PxCMD_ST));
    self->port->ci = 1u << slot;

    // wait for completion
    memory_fence();
    while(true) {
        if((self->port->ci & (1u << slot)) == 0) {
            break;
        }else if(self->port->is & AHCI_PxIS_TFES) {
            ASSERT(false);
        }
        cpu_pause();
    }

    // prepare the model name
    char buffer[48] = {0};
    for(int i = 0; i < 27; i++) {
        buffer[i * 2 + 0] = (char)((identify->model_number[i] >> 8u) & 0xFFu);
        buffer[i * 2 + 1] = (char)(identify->model_number[i] & 0xFFu);
    }

    // now do stuff with the identify data
    debug_log("[*] ahci: \t%s\n", buffer);

    // free the buffers
    mm_free_pages((void*)identify, SIZE_TO_PAGES(sizeof(ATA_IDENTIFY_DATA)));
    mm_free_pages(cmdtbl, SIZE_TO_PAGES(offsetof(HBA_CMD_TBL, prdt_entry) + sizeof(cmd->prdtl)));

    return _self;
}

static void* AhciDevice_dtor(void* _self) {
    __attribute__((unused)) ahci_device_t* self = super_dtor(AhciDevice(), _self);
    return _self;
}

static size_t AhciDevice_get_block_size(void* _self) {
//    ahci_device_t* device = cast(AhciDevice(), _self);

    return 0;
}

static size_t AhciDevice_read_block(void* _self, uintptr_t lba, void* buffer) {
//    ahci_device_t* device = cast(AhciDevice(), _self);
    ASSERT(false);
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
