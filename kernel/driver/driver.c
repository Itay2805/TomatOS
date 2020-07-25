#include <util/stb_ds.h>
#include "driver.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Few helper functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static uint32_t bswap32(uint32_t dword) {
    return (uint32_t)((dword>>24) & 0xFF)
           | ((dword<<8) & 0xFF0000)
           | ((dword>>8)&0xFF00)
           | ((dword<<24)&0xFF000000);
}

static uint8_t char_to_hex(char character) {
    if (character <= '9')
        return character - '0';
    else if(character >= 'A' && character <= 'F')
        return character - 'A' + 10;
    else if(character >= 'a' && character <= 'f')
        return character - 'a' + 10;

    return 0;
}

static uint32_t str_to_eisa(const char* id) {
    uint32_t out = 0;
    out |= ((id[0] - 0x40) << 26);
    out |= ((id[1] - 0x40) << 21);
    out |= ((id[2] - 0x40) << 16);
    out |= char_to_hex(id[3]) << 12;
    out |= char_to_hex(id[4]) << 8;
    out |= char_to_hex(id[5]) << 4;
    out |= char_to_hex(id[6]);

    out = bswap32(out);
    return out;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Generic binding
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct driver_entry {
    driver_t* driver;
    driver_bind_t* bind;
} driver_entry_t;



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ACPI Drivers
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static struct {
    uint32_t key;
    driver_entry_t value;
}* g_acpi_drivers = NULL;

static bool check_acpi_id(lai_nsnode_t* node, lai_nsnode_t* hid_handle) {
    LAI_CLEANUP_STATE lai_state_t state;
    LAI_CLEANUP_VAR lai_variable_t id = LAI_VAR_INITIALIZER;
    lai_init_state(&state);

    if (hid_handle) {
        if (lai_eval(&id, hid_handle, &state)) {
            WARN(false, "could not evaluate _HID of device");
        } else if (id.type == LAI_INTEGER) {
            int index = hmgeti(g_acpi_drivers, id.integer);
            if (index != -1) {
                driver_entry_t entry;
                char* path = lai_stringify_node_path(node);
                TRACE("\tBound `%s`", entry.driver->name);
                TRACE("\t\tID: %s", entry.bind->acpi.hid);
                TRACE("\t\tPATH: %s", path);
                kfree(path);
                return true;
            }
        }
    }

    return false;
}

static void bind_acpi_drivers() {
    TRACE("Binding ACPI drivers");

    struct lai_ns_iterator iter = LAI_NS_ITERATOR_INITIALIZER;
    lai_nsnode_t* node;
    while ((node = lai_ns_iterate(&iter))) {
        // try to get the _HID
        lai_nsnode_t* hid_handle = lai_resolve_path(node, "_HID");
        if (check_acpi_id(node, hid_handle)) continue;

        // if we got here the _HID did not go well, try _CID
        lai_nsnode_t *cid_handle = lai_resolve_path(node, "_CID");
        if (check_acpi_id(node, cid_handle)) continue;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Driver dispatching
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

err_t dispatch_drivers() {
    err_t err = NO_ERROR;

     size_t driver_count = g_drivers_end - g_drivers;

    // prepare drivers
    for (int i = 0; i < driver_count; i++) {
        driver_t* driver = &g_drivers[i];
        for (int j = 0; j < driver->binds_count; j++) {
            switch (driver->binds[j].type) {

                case DRIVER_ACPI: {
                    driver_entry_t data = {
                        .driver = driver,
                        .bind = &driver->binds[j],
                    };
                    hmput(g_acpi_drivers, str_to_eisa(driver->binds[j].acpi.hid), data);
                } break;

                default: break;
            }
        }
    }

    bind_acpi_drivers();

    hmfree(g_acpi_drivers);
    return err;
}
