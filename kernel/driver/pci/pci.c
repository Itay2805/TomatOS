#include <lai/helpers/pci.h>

#include <driver/driver.h>
#include <util/stb_ds.h>
#include <mem/vmm.h>

#include "pci_spec.h"
#include "pci.h"

static acpi_mcfg_entry_t* g_mcfg_entries = NULL;
static size_t g_mcfg_entry_count = 0;

pci_dev_t** g_pci_roots = NULL;
pci_entry_t* g_pci_map = NULL;

err_t early_init_pci() {
    err_t err = NO_ERROR;

    acpi_mcfg_t* mcfg = laihost_scan("MCFG", 0);
    CHECK_ERROR_TRACE(mcfg != NULL, ERROR_NOT_FOUND, "We only support memory mapped pci!");
    g_mcfg_entry_count = (mcfg->header.length - offsetof(acpi_mcfg_t, entries)) / sizeof(acpi_mcfg_entry_t);
    g_mcfg_entries = mcfg->entries;

cleanup:
    return err;
}

static err_t enum_bus(pci_dev_t* root_dev, uint8_t bus, uint8_t start);

/**
 * Will initialize the device, assumes that the following is already set:
 * - address
 * - parent
 * - header
 */
static err_t init_pci_dev(pci_dev_t* dev) {
    err_t err = NO_ERROR;
    uint8_t bar_count;
    volatile uint32_t* bars;

    // add to the map
    hmput(g_pci_map, dev->address, dev);

    // common info
    dev->device_id = dev->header->device_id;
    dev->vendor_id = dev->header->vendor_id;
    dev->class = dev->header->class;
    dev->subclass = dev->header->subclass;
    dev->progif = dev->header->progif;

    TRACE("%04x:%02x:%02x.%x %s <%04x:%04x>",
          dev->address.seg, dev->address.bus,
          dev->address.slot, dev->address.func,
          get_pci_name(dev->class, dev->subclass, dev->progif),
          dev->device_id, dev->vendor_id);

    // figure bar count
    uint8_t header_type = dev->header->header_type & 0x7f;
    switch (header_type) {
        case 0x00: {
            if (dev->parent && dev->parent->acpi_node != NULL) {
                LAI_CLEANUP_STATE lai_state_t state;
                lai_init_state(&state);
                dev->acpi_node = lai_pci_find_device(dev->parent->acpi_node, dev->address.slot, dev->address.func, &state);
                if (dev->acpi_node) {
                    char* name = lai_stringify_node_path(dev->acpi_node);
                    TRACE("\tACPI: %s", name);
                    kfree(name);
                }
            }

            bar_count = 6;
            bars = dev->device->bar;
        } break;

        case 0x01: {
            LAI_CLEANUP_STATE lai_state_t state;
            lai_init_state(&state);

            dev->acpi_node = lai_pci_find_bus(dev->address.seg, dev->address.bus, &state);
            if (dev->acpi_node) {
                char* name = lai_stringify_node_path(dev->acpi_node);
                TRACE("\tACPI: %s", name);
                kfree(name);
            }

            bar_count = 2;
            bars = dev->bridge->bar;
        } break;

        default:
            CHECK_FAIL_TRACE("Unsupported header type %x", header_type);
    }

    // parse bridges
    for (int i = 0; i < bar_count; i++) {
        uint32_t bar = bars[i];
        if (bar & BIT0) {
            dev->bars[i].present = true;
            dev->bars[i].mmio = false;
            dev->bars[i].port = bar & 0xFFFC;

            bars[i] = UINT32_MAX;
            dev->bars[i].length = ~(bars[i] & ~(0x3)) + 1;
            dev->bars[i].length &= 0xFFFF;
            bars[i] = bar;

            TRACE("\tIO: 0x%x-0x%x", dev->bars[i].port, dev->bars[i].port + dev->bars[i].length);
        } else {
            uint8_t bar_type = (bar >> 1) & 0x3;
            if (bar_type == 0) {
                uint32_t addr = bar & 0xFFFFFFF0;
                if (addr != 0) {
                    dev->bars[i].present = true;
                    dev->bars[i].mmio = true;
                    dev->bars[i].addr = PHYSICAL_TO_DIRECT((physptr_t)addr);

                    bars[i] = UINT32_MAX;
                    dev->bars[i].length = ~(bars[i] & ~(0x3)) + 1;
                    bars[i] = bar;

                    TRACE("\tMMIO: 0x%x-0x%x", dev->bars[i].addr, dev->bars[i].addr + dev->bars[i].length);
                }
            } else if (bar == 2) {
                TRACE("\tMMIO: TODO 64bit");
                i++;
            }
        }
    }

    dev->irq = -1;
    if (dev->device->interrupt_pin) {
        lai_nsnode_t* prt_node = NULL;
        LAI_CLEANUP_VAR lai_variable_t prt = LAI_VAR_INITIALIZER;
        LAI_CLEANUP_STATE lai_state_t state;
        lai_init_state(&state);

        uint8_t pin = dev->device->interrupt_pin;
        pci_dev_t* tmp = dev;
        while (1) {
            if (tmp->parent != NULL) {
                if (tmp->parent->acpi_node != NULL) {
                    prt_node = lai_resolve_path(tmp->parent->acpi_node, "_PRT");
                    if (prt_node != NULL) {
                        break;
                    }
                }
                pin = (((pin - 1) + (tmp->address.slot % 4)) % 4) + 1;
                tmp = tmp->parent;
            } else {
                CHECK(tmp->acpi_node);
                prt_node = lai_resolve_path(tmp->acpi_node, "_PRT");
                break;
            }
        }

        // eval the prt
        CHECK_ERROR(prt_node != NULL, ERROR_NOT_FOUND);
        CHECK_LAI(lai_eval(&prt, prt_node, &state));

        // iterate the prt
        struct lai_prt_iterator iter = LAI_PRT_ITERATOR_INITIALIZER(&prt);
        lai_api_error_t e;
        while(!(e = lai_pci_parse_prt(&iter))) {
            if (iter.slot == tmp->address.slot &&
                (iter.function == -1 || iter.function == tmp->address.func) &&
                iter.pin == pin - 1) {
                dev->irq = iter.gsi;
                TRACE("\tIRQ: %d", dev->irq);
                break;
            }
        }
        CHECK_LAI(e);
    }

    // pci-to-pci bridge, iterate over the second bridge
    if (dev->class == PCI_CLASS_BRIDGE && dev->subclass == PCI_CLASS_BRIDGE_P2P) {
        CHECK_AND_RETHROW(enum_bus(dev, dev->bridge->secondary_bus, 0));
    }

cleanup:
    return err;
}

/**
 * Iterate the bus and functions of the given device
 */
static err_t enum_bus(pci_dev_t* root_dev, uint8_t bus, uint8_t start) {
    err_t err = NO_ERROR;

    // now iterate the rest of the devices
    for (uint8_t slot = start; slot < 32; slot++) {
        volatile pci_common_header_t* header;
        CHECK_AND_RETHROW(get_config_for_pci(root_dev->address.seg, bus, slot, 0, (void**)&header));
        if (header->vendor_id == 0xFFFF) continue;

        // there is a device, iterate functions
        uint8_t multifunc = header->header_type & 0x80;
        for (int func = 0; func < (multifunc ? 8 : 1); func++) {
            CHECK_AND_RETHROW(get_config_for_pci(root_dev->address.seg, bus, slot, func, (void**)&header));
            if (header->vendor_id == 0xFFFF) continue;

            pci_dev_t* dev = kalloc(sizeof(pci_dev_t));
            dev->parent = root_dev;
            dev->header = header;
            dev->device = (void*)(dev->header + 1);
            dev->address.seg = root_dev->address.seg;
            dev->address.bus = bus;
            dev->address.slot = slot;
            dev->address.func = func;
                    arrpush(root_dev->childs, dev);
            CHECK_AND_RETHROW(init_pci_dev(dev));
        }
    }

    cleanup:
    return err;
}

/**
 * PCI driver entry
 *
 * this will enumarate everything correctly
 */
err_t pci_entry(driver_bind_data_t* data) {
    err_t err = NO_ERROR;
    LAI_CLEANUP_STATE lai_state_t state;
    lai_init_state(&state);

    ASSERT(data->bind->type == DRIVER_ACPI);
    lai_nsnode_t* node = data->acpi.node;

    uint64_t bus_result = 0;
    lai_nsnode_t* bbn_handle = lai_resolve_path(node, "_BBN");
    if (bbn_handle) {
        LAI_CLEANUP_VAR lai_variable_t bus_number = LAI_VAR_INITIALIZER;
        CHECK_LAI(lai_eval(&bus_number, bbn_handle, &state));
        CHECK_LAI(lai_obj_get_integer(&bus_number, &bus_result));
    }

    uint64_t seg_result = 0;
    lai_nsnode_t* seg_handle = lai_resolve_path(node, "_SEG");
    if (seg_handle) {
        LAI_CLEANUP_VAR lai_variable_t seg_number = LAI_VAR_INITIALIZER;
        CHECK_LAI(lai_eval(&seg_number, seg_handle, &state));
        CHECK_LAI(lai_obj_get_integer(&seg_number, &seg_result));
    }

    // create the host device
    pci_dev_t* root_dev = kalloc(sizeof(pci_dev_t));
    CHECK_ERROR(root_dev != NULL, ERROR_OUT_OF_RESOURCES);
    CHECK_AND_RETHROW(get_config_for_pci(seg_result, bus_result, 0, 0, (void**)&root_dev->header));
    root_dev->device = (void*)(root_dev->header + 1);
    root_dev->address.seg = seg_result;
    root_dev->address.bus = bus_result;
    root_dev->acpi_node = node;
    CHECK_AND_RETHROW(init_pci_dev(root_dev));
    arrpush(g_pci_roots, root_dev);

    CHECK_AND_RETHROW(enum_bus(root_dev, root_dev->address.bus, 1));

cleanup:
    return err;
}

DRIVER {
    .name = "PCI/PCIe Bus",
    .entry = pci_entry,
    .binds = (driver_bind_t[]) {
        /* PCI Bus */
        {
            .type = DRIVER_ACPI,
            .acpi = {
                .hid = "PNP0A03"
            }
        },
        /* PCI Express Bus */
        {
            .type = DRIVER_ACPI,
            .acpi = {
                .hid = "PNP0A08"
            }
        },
        { DRIVER_END }
    }
};

err_t get_config_for_pci(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, void** config) {
    err_t err = NO_ERROR;

    CHECK(config != 0);

    // check if we have the device in the hashmap
    pci_address_t addr = (pci_address_t){ seg, bus, slot, fun };
    pci_dev_t* dev = hmget(g_pci_map, addr);
    if (dev != NULL) {
        *config = (void*)dev->header;
        goto cleanup;
    } else {
        // otherwise lookup in the mcfg
        CHECK_ERROR(g_mcfg_entries != NULL, ERROR_NOT_READY);
        for (int i = 0; i < g_mcfg_entry_count; i++) {
            if (g_mcfg_entries[i].pci_segment == seg && g_mcfg_entries[i].start_bus <= bus && bus <= g_mcfg_entries[i].end_bus) {
                *config = PHYSICAL_TO_DIRECT(g_mcfg_entries[i].base_address + ((bus - g_mcfg_entries[i].start_bus) << 20 | slot << 15 | fun << 12));
                goto cleanup;
            }
        }
    }

    CHECK_FAIL_ERROR(ERROR_NOT_FOUND);

cleanup:
    return err;
}
