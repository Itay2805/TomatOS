#include <driver/driver.h>
#include <util/defs.h>

#undef  __MODULE__
#define __MODULE__ "atrtc"

static void rtc_entry(driver_bind_data_t* data) {
}

DRIVER {
    .name = "RTC",
    .entry = rtc_entry,
    .binds = (driver_bind_t[]) {
        {
            /* AT Real-Time Clock */
            .type = DRIVER_ACPI,
            .acpi = {
                .hid = "PNP0B00"
            }
        },
        { DRIVER_END }
    },
};
