#include <util/defs.h>
#include <driver/driver.h>
#include <driver/pci/pci_spec.h>

static err_t echfs_connect(driver_interface_t* interface) {
    return NO_ERROR;
}

static bool echfs_check(driver_interface_t* interface) {
    return false;
}

static err_t echfs_entry(driver_bind_data_t* data) {
    return true;
}

DRIVER {
    .name = "ECHFS",
    .entry = echfs_entry,
    .interface = {
        .type = DRIVER_FS,
    },
    .binds = (driver_bind_t[]) {
        {
            .type = BIND_INTERFACE,
            .interface = {
                .type = DRIVER_BLOCK,
                .connect = (driver_connect_t) echfs_connect,
                .check = (driver_check_t) echfs_check,
            }
        },
        {BIND_END }
    }
};