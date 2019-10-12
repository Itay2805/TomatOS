#include <pci/pci.h>
#include <stb/stb_ds.h>
#include <common.h>
#include <helpers/hpet/hpet.h>
#include <objects/object.h>
#include <memory/mm.h>
#include <memory/pmm.h>
#include "ahci.h"
#include "ahci_spec.h"

ahci_device_t* ahci_devices;

///////////////////////////////////////////////////////
// Reading and writing AHCI registers
///////////////////////////////////////////////////////

static void ahci_write(ahci_device_t* dev, size_t offset, uint32_t value) {
    VOL_POKE32(dev->bar->base + offset) = value;
}

static uint32_t ahci_read(ahci_device_t* dev, size_t offset) {
    return POKE32(dev->bar->base + offset);
}

static uint32_t ahci_vol_read(ahci_device_t* dev, size_t offset) {
    return VOL_POKE32(dev->bar->base + offset);
}

///////////////////////////////////////////////////////
// AHCI initialization
///////////////////////////////////////////////////////

static error_t do_ahci_handoff(ahci_device_t* dev) {
    error_t err = NO_ERROR;

    if(ahci_read(dev, AHCI_CAP2) & AHCI_CAP2_BOH) {
        log_info("\t\tTaking ownership");
        ahci_write(dev, AHCI_BOHC, ahci_read(dev, AHCI_BOHC) | AHCI_BOHC_OOS);

        // poll for 2sec
        bool owned = false;
        uint64_t start = hpet_get_millis();
        while(true) {
            // check if we own it now
            uint32_t handoff = ahci_vol_read(dev, AHCI_BOHC);
            if((handoff & AHCI_BOHC_OOS) && !(handoff & AHCI_BOHC_BOS)) {
                break;
            }

            // timeout in 2 seconsd
            if(hpet_get_millis() - start > 1000) {
                break;
            }
        }

        CHECK_TRACE(owned, "Failed to take AHCI ownership!");
    }

cleanup:
    return err;
}

static error_t init_ahci_port(ahci_device_t* dev, size_t port) {
    error_t err = NO_ERROR;

    ahci_port_t* new_port = &dev->ports[port];

    // make sure this is a supported type
    int type = ahci_read(dev, AHCI_PxSIG(port));
    if(type != AHCI_SIGNATURE_SATA) {
        goto cleanup;
    }

    // create the object
    object_t* obj = vmalloc(sizeof(object_t));
    obj->type = OBJECT_STORAGE;
    obj->name = "ahci";
    obj->context = new_port;
    // TODO: setup syscalls and functions

    new_port->obj = obj;

    CHECK_AND_RETHROW(object_add(obj));

    // allocate buffers
    new_port->recevied_fis_base = pmalloc(1);
    new_port->command_list_base = pmalloc(1);

    CHECK(new_port->recevied_fis_base != NULL);
    CHECK(new_port->command_list_base != NULL);

    uintptr_t clb_phys = (uintptr_t) (new_port->command_list_base - DIRECT_MAPPING_BASE);
    uintptr_t rfb_phys = (uintptr_t) (new_port->recevied_fis_base - DIRECT_MAPPING_BASE);

    // set the addresses
    ahci_write(dev, AHCI_PxCLB(port), (uint32_t) (clb_phys & 0xFFFFFFFF));
    ahci_write(dev, AHCI_PxCLBU(port), (uint32_t) ((clb_phys >> 32) & 0xFFFFFFFF));
    ahci_write(dev, AHCI_PxFB(port), (uint32_t) (rfb_phys & 0xFFFFFFFF));
    ahci_write(dev, AHCI_PxFBU(port), (uint32_t) ((rfb_phys >> 32) & 0xFFFFFFFF));

    // handle port specific stuff
    switch(type) {
        case AHCI_SIGNATURE_SATA: {
            log_info("\t\tgot SATA at port #%d", port);
            new_port->type = AHCI_PORT_SATA;
            // TODO: Identify
        } break;

//        case AHCI_SIGNATURE_SATAPI: {
//            log_info("\t\tgot SATAPI at port #%d", port);
//            new_port->type = AHCI_PORT_SATAPI;
//            // TODO: Identify
//        } break;

        default:
            // should not reach here
            CHECK_FAIL();
    }

cleanup:
    if(err != NO_ERROR) {
        // free the objects
        vfree(obj);

        // zero out the buffer
        memset(new_port, 0, sizeof(*new_port));

        // free the buffers
        pfree(new_port->command_list_base, 1);
        pfree(new_port->recevied_fis_base, 1);
    }
    return err;
}

static error_t init_ahci_device(pci_dev_t* dev) {
    error_t err = NO_ERROR;

    ahci_device_t ahci_dev = {
        .dev = dev,
        .bar = &dev->bars[arrlen(dev->bars) - 1]
    };

    // remap this with the real length
    ahci_dev.bar->len = KB(2);
    CHECK_AND_RETHROW(vmm_map_direct(ahci_dev.bar->base - DIRECT_MAPPING_BASE, ahci_dev.bar->len, (page_attrs_t){ .write = true, .global = true }));

    // take control
    CHECK_AND_RETHROW(do_ahci_handoff(&ahci_dev));

    // initialize available ports
    // TODO: only iterate to max ports
    uint32_t ports = ahci_read(&ahci_dev, AHCI_PI);
    for(size_t i = 0; i < 32; i++) {
        if(ports & (1 << i)) {
            CATCH(init_ahci_port(&ahci_dev, i));
        }
    }

cleanup:
    return err;
}

///////////////////////////////////////////////////////
// List of supported devices and their names
///////////////////////////////////////////////////////


static pci_sig_t supported_devices[] = {
    { 0x8086, 0x2922 },
    { 0x8086, 0x2829 },
    { 0x8086, 0xa102 }
};

static const char* supported_devices_names[] = {
    "Intel 82801IR/IO/IH (ICH9R/DO/DH) 6 port SATA Controller [AHCI mode]",
    "Intel 82801HM/HEM (ICH8M/ICH8M-E) SATA Controller [AHCI mode]",
    "Q170/Q150/B150/H170/H110/Z170/CM236 Chipset SATA Controller [AHCI Mode]"
};

///////////////////////////////////////////////////////
// driver entry
///////////////////////////////////////////////////////

error_t ahci_init() {
    error_t err = NO_ERROR;

    for(int i = 0; i < arrlen(pci_devices); i++) {
        pci_dev_t* dev = pci_devices[i];

        // Identify by class and type
        bool supported = false;
        for(int j = 0; j < (sizeof(supported_devices) / sizeof(pci_sig_t)); j++) {
            if(supported_devices[j].device_id == dev->device_id && supported_devices[j].vendor_id == dev->vendor_id) {
                log_info("\t%s at %x.%x.%x.%x", supported_devices_names[j], dev->segment, dev->bus, dev->device, dev->function);
                supported = true;
                break;
            }
        }

        if(!supported) {
            // check if supported by type
            if(dev->class == 0x01 && dev->subclass == 0x06 && dev->prog_if == 0x01) {
                log_warn("\tUnknown AHCI device at %x.%x.%x.%x (%x:%x), initializing anyways", dev->segment, dev->bus, dev->device, dev->function, dev->vendor_id, dev->device_id);
                supported = true;
            }
        }

        if(supported) {
            CATCH(init_ahci_device(dev));
        }
    }

cleanup:
    return err;
}
