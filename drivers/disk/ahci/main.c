#include <util/defs.h>
#include <driver/driver.h>
#include <driver/pci/pci_spec.h>

static err_t ahci_entry(driver_bind_data_t* data) {
    err_t err = NO_ERROR;
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