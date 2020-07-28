#include <driver/driver.h>
#include <util/defs.h>

#undef  __MODULE__
#define __MODULE__ "ps2"

err_t ps2kbd_entry(driver_bind_data_t* data) {
    return NO_ERROR;
}

err_t ps2mou_entry(driver_bind_data_t* data) {
    return NO_ERROR;
}

DRIVER {
    .name = "Standard PS2 Keyboard",
    .entry = ps2kbd_entry,
    .binds = (driver_bind_t[]) {
        {
            /* IBM Enhanced Keyboard (101/102-key, PS/2 Mouse) */
            .type = DRIVER_ACPI,
            .acpi = {
                .hid = "PNP0303"
            }
        },
        { DRIVER_END }
    },
};

DRIVER {
    .name = "Standard PS2 Mouse",
    .entry = ps2mou_entry,
    .binds = (driver_bind_t[]) {
        {
            /* PS/2 Mouse */
            .type = DRIVER_ACPI,
            .acpi = {
                .hid = "PNP0F13"
            }
        },
        { DRIVER_END }
    },
};
