#include <pci/pci_ids.h>
#include <pci/pci.h>
#include <stb/stb_ds.h>
#include "rtl8139.h"
#include "rtl8139_spec.h"

#include "../registers.h"

typedef struct rtl8139_dev {
    pci_dev_t* dev;
    uintptr_t mmio_base;
} rtl8139_dev_t;

GENERATE_REGISTER_READ_WRITE(rtl8139_dev_t, mmio_base);

static error_t init_device(pci_dev_t* pcidev) {
    error_t err = NO_ERROR;

    CHECK(pcidev != NULL);

    rtl8139_dev_t* dev = vmalloc(sizeof(rtl8139_dev_t));
    CHECK(dev != NULL);

    dev->dev = pcidev;
    dev->mmio_base = pcidev->bars[1].base;

    log_info("\t\tMAC: %x:%x:%x:%x:%x:%x",
         read8_relaxed(dev, IDR0),
         read8_relaxed(dev, IDR1),
         read8_relaxed(dev, IDR2),
         read8_relaxed(dev, IDR3),
         read8_relaxed(dev, IDR4),
         read8_relaxed(dev, IDR5)
    );

cleanup:
    if(err != NO_ERROR) {
        if(dev != NULL) {
            vfree(dev);
        }
    }
    return err;
}

static pci_sig_t supported_devices[] = {
    { PCI_VENDOR_REALTEK, 0x8139 },
};

static const char* supported_devices_names[] = {
    "rtl8139"
};

error_t rtl8139_init() {
    error_t err = NO_ERROR;

    for(int i = 0; i < arrlen(pci_devices); i++) {
        pci_dev_t* dev = pci_devices[i];

        // Identify by type
        for(int j = 0; j < (sizeof(supported_devices) / sizeof(pci_sig_t)); j++) {
            if(supported_devices[j].device_id == dev->device_id && supported_devices[j].vendor_id == dev->vendor_id) {
                log_info("\t%s at %x.%x.%x.%x (rev %x)", supported_devices_names[j], dev->segment, dev->bus, dev->device, dev->function, dev->revision_id);
                CATCH(init_device(dev));
                break;
            }
        }
    }

cleanup:
    return err;
}
