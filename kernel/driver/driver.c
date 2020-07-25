#include "driver.h"

err_t dispatch_drivers() {
    err_t err = NO_ERROR;

     size_t driver_count = g_drivers_end - g_drivers;

    TRACE("Preparing drivers (total of %d)", driver_count);
    for (int i = 0; i < driver_count; i++) {
        driver_t* driver = &g_drivers[i];
        TRACE("\t%s:", driver->name);
        for (int j = 0; j < driver->binds_count; j++) {
            switch (driver->binds[j].type) {
                case DRIVER_ACPI: {
                    TRACE("\t\tACPI: %s", driver->binds[j].acpi.hid);
                } break;

                default: break;
            }
        }
    }

//cleanup:
    return err;
}
