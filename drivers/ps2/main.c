
#include <driver/driver.h>
#include <util/defs.h>

void ps2_entry(driver_bind_data_t* data) {
    TRACE("PS2 Driver started!");
}

static driver_bind_t g_binds[] = {
    {
        .type = DRIVER_ACPI,
        .acpi = {
            .hid = "PNP0303"
        }
    }
};

DRIVER {
    .name = "Standard PS2 Keyboard",
    .entry = ps2_entry,
    .binds_count = ARRAY_LENGTH(g_binds),
    .binds = (driver_bind_t*)&g_binds,
};
