#include <util/stb_ds.h>
#include <lai/helpers/sci.h>
#include <util/defs.h>
#include <proc/process.h>
#include <proc/scheduler.h>
#include <driver/pci/pci.h>
#include "driver.h"

list_entry_t g_interfaces[DRIVER_MAX];
ticket_lock_t g_interfaces_locks[DRIVER_MAX];

static const char* g_interface_names[] = {
    [DRIVER_KEYBOARD] = "Keyboard",
    [DRIVER_MOUSE] = "Mouse",
    [DRIVER_STORAGE] = "Storage",
    [DRIVER_BLOCK] = "Block",
    [DRIVER_FS] = "Filesystem",
};

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
        uint64_t index;
        if (lai_eval(&id, hid_handle, &state) != LAI_ERROR_NONE) {
            WARN(false, "could not evaluate _HID of device");
        } else if (lai_obj_get_type(&id) == LAI_INTEGER && lai_obj_get_integer(&id, &index) == LAI_ERROR_NONE) {
            int i = hmgeti(g_acpi_drivers, id.integer);
            if (i != -1) {
                // everything is good!
                driver_entry_t* entry = &g_acpi_drivers[i].value;
                char* path = lai_stringify_node_path(node);
                TRACE("Bound `%s`", entry->driver->name);
                TRACE("\tID: %s", entry->bind->acpi.hid);
                TRACE("\tPATH: %s", path);
                kfree(path);

                // start it
                driver_bind_data_t data;
                data.bind = entry->bind;
                data.acpi_node = node;
                WARN(!IS_ERROR(entry->driver->entry(&data)), "Got error while initializing driver, ignoring");

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
        if (lai_ns_get_node_type(node) != LAI_NODETYPE_DEVICE) continue;

        // try to get the _HID
        lai_nsnode_t* hid_handle = lai_resolve_path(node, "_HID");
        if (check_acpi_id(node, hid_handle)) continue;

        // if we got here the _HID did not go well, try _CID
        lai_nsnode_t *cid_handle = lai_resolve_path(node, "_CID");
        if (check_acpi_id(node, cid_handle)) continue;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PCI drivers
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void bind_pci_drivers() {
    TRACE("Binding PCI drivers");

    size_t driver_count = g_drivers_end - g_drivers;
    for (int i = 0; i < driver_count; i++) {
        driver_t *driver = &g_drivers[i];
        driver_bind_t *bind = driver->binds;
        while (bind->type != BIND_END) {
            switch (bind->type) {
                case BIND_PCI: {
                    for (int j = 0; j < hmlen(g_pci_map); j++) {
                        if (g_pci_map[j].value == NULL) continue;
                        pci_dev_t* dev = g_pci_map[j].value;

                        // first check out the class-subclass-progif
                        if (bind->pci.class != 0 && bind->pci.class != dev->class) continue;
                        if (bind->pci.subclass != 0 && bind->pci.subclass != dev->subclass) continue;
                        if (bind->pci.progif != 0 && bind->pci.progif != dev->progif) continue;
                        if (bind->pci.device_id != 0 && bind->pci.device_id != dev->device_id) continue;
                        if (bind->pci.vendor_id != 0 && bind->pci.vendor_id != dev->vendor_id) continue;

                        // match!
                        TRACE("Bound `%s`", driver->name);
                        TRACE("\tID: %04x:%04x", dev->device_id, dev->vendor_id);
                        TRACE("\tPATH: %04x:%02x:%02x.%x",
                                dev->address.seg, dev->address.bus,
                                dev->address.slot, dev->address.func);

                        // TODO: don't double bind on acpi nodes

                        // start the thread
                        // start it
                        driver_bind_data_t data;
                        data.bind = bind;
                        data.pci_dev = dev;
                        WARN(!IS_ERROR(driver->entry(&data)), "Got error while initializing driver, ignoring");

                    }
                } break;

                default: break;
            }
            bind++;
        }
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Driver dispatching
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

err_t dispatch_drivers() {
    err_t err = NO_ERROR;

    // init drivers list

    size_t driver_count = g_drivers_end - g_drivers;
    TRACE("Preparing driver dispatch");
    for (int i = 0; i < driver_count; i++) {
        driver_t* driver = &g_drivers[i];
        driver_bind_t* bind = driver->binds;
        while (bind->type != BIND_END) {
            switch (bind->type) {
                case BIND_ACPI: {
                    driver_entry_t data = {
                        .driver = driver,
                        .bind = bind,
                    };
                    hmput(g_acpi_drivers, str_to_eisa(bind->acpi.hid), data);
                } break;

                default: break;
            }
            bind++;
        }
    }

    bind_acpi_drivers();
    bind_pci_drivers();

    TRACE("Finished driver dispatch");

    hmfree(g_acpi_drivers);
    return err;
}

err_t register_interface(driver_instance_t* instance) {
    err_t err = NO_ERROR;

    CHECK(instance != NULL);
    CHECK(instance->type < DRIVER_MAX);

    // add to driver list
    ticket_lock(&g_interfaces_locks[instance->type]);
    list_add(&g_interfaces[instance->type], &instance->link);
    ticket_unlock(&g_interfaces_locks[instance->type]);

    // notify anyone who wants to know about this
    size_t driver_count = g_drivers_end - g_drivers;
    for (int i = 0; i < driver_count; i++) {
        driver_t* driver = &g_drivers[i];
        for (driver_bind_t* bind = driver->binds; bind->type != BIND_END; bind++) {
            if (bind->type == BIND_INTERFACE && bind->interface.type == instance->type) {
                if (bind->interface.check(instance)) {
                    TRACE("Bound `%s`", driver->name);
                    TRACE("\tTYPE: %s", g_interface_names[bind->interface.type]);

                    instance->meta.refcount++;
                    WARN(!IS_ERROR(bind->interface.connect(instance)), "Error connecting interface!");
                    break;
                }
            }
        }
    }

    // TODO: notify userspace for a
    //       new interface connection

cleanup:
    return err;
}
