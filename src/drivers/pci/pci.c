#include <acpi/tables/mcfg.h>
#include <memory/vmm.h>
#include <util/defs.h>
#include <processes/process.h>
#include <lai/core.h>
#include <stb/stb_ds.h>
#include "pci.h"
#include "pci_spec.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The pci device object
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void* PciDevice_ctor(void* _self, va_list ap) {
    pci_device_t* self = super_ctor(PciDevice(), _self, ap);

    // get the arguments
    pci_device_t* parent = va_arg(ap, pci_device_t*);
    uint16_t seg = va_arg(ap, uint32_t);
    uint8_t bus = va_arg(ap, uint32_t);
    uint8_t slot = va_arg(ap, uint32_t);
    uint8_t func = va_arg(ap, uint32_t);
    
    // set the stuff
    self->parent = parent;
    self->segment = seg;
    self->bus = bus;
    self->slot = slot;
    self->function = func;

    // get the base
    self->mmio_base = (uintptr_t) pci_get_config_space(seg, bus, slot, func);

    // figure the bars
    uint32_t* bars;
    size_t count;
    switch(self->header->header_type) {
        case 0:
            bars = &self->device_header->bar0;
            count = 6;
            break;

        case 2:
            bars = &self->bridge_header->bar0;
            count = 2;
            break;

        default:
            bars = NULL;
            count = 0;
            break;
    }

    if(bars != NULL) {
        for(size_t i = 0; i < count; i++) {
            if(bars[i] & 1u) {
                self->bars[i].port = true;
                self->bars[i].base = bars[i] & ~0b11u;

                // TODO: limit
            }else {
                self->bars[i].port = false;
                self->bars[i].base = bars[i] & ~0b1111u;

                // 64bit bar
                if((bars[i] >> 1u) == 2) {
                    self->bars[i].base = (uintptr_t)bars[i + 1] << 32u;
                    i++;
                }

                // TODO: limit
            }

            // convert to direct mapping (TODO: might need to map this eventually)
            if(self->bars[i].base != 0) {
                self->bars[i].base = PHYSICAL_TO_DIRECT(self->bars[i].base);
            }
        }
    }


    // figure caps that we want to know about
    // only run if caps are supported
    if(self->header->status.caps_list) {

        uint8_t current_off = self->header->caps_pointer;
        do {
            uint8_t cap_id = POKE8(self->mmio_base + current_off + 0);
            uint8_t next_ptr = POKE8(self->mmio_base + current_off + 1);

            switch(cap_id) {

                // msi
                case 0x05: {
                    self->msi = (msi_cap_t*)(self->mmio_base + current_off);
                } break;

                    // TODO: more caps we want to support

                default:
                    break;
            }

            current_off = next_ptr;
        } while(current_off != 0);
    }

    // TODO: irq routing
    self->irq = -1;

    return self;
}

static void* PciDevice_dtor(void* _self) {
    pci_device_t* self = super_dtor(PciDevice(), _self);
    return self;
}

const void* PciDevice() {
    static const void* class = NULL;
    if(class == NULL) {
        class = new(Class(),
                "PciDevice", Object(), sizeof(pci_device_t),
                ctor, PciDevice_ctor,
                dtor, PciDevice_dtor);
    }
    return class;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PCI Scanning
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

list_entry_t pci_devices = INIT_LIST_ENTRY(pci_devices);
static pci_device_t** bus_stack = NULL;

static void scan_bus(uint16_t segment, uint8_t bus);

static void init_device(uint16_t segment, uint8_t bus, uint8_t slot, uint8_t func) {
    // get the parent
    pci_device_t* parent = NULL;
    if(arrlen(bus_stack) != 0) {
        parent = arrlast(bus_stack);
    }

    // make sure the device is good
    uint16_t* base = pci_get_config_space(segment, bus, slot, func);
    ASSERT(base != NULL);
    if(*base == 0xFFFF) {
        return;
    }

    // initialize the device
    pci_device_t* device = new(PciDevice(), parent, segment, bus, slot, func);

    // log it nicely
    debug_log("[*] \t%04x:%02x:%02x.%x -> %s (%04x:%04x)\n",
            segment, bus, slot, 0,
            pci_get_name(device->header->class_code, device->header->subclass_code, device->header->program_interface),
            device->header->vendor_id, device->header->device_id);

    if(device->msi != NULL) {
        debug_log("[*] \t\tSupports MSI\n");
    }

    if(device->irq != -1) {
        debug_log("[*] \t\tIRQ #%d\n", device->irq);
    }

    for(int i = 0; i < 6; i++) {
        if(device->bars[i].base != 0) {
            debug_log("[*] \t\t%s #%d: %p\n", device->bars[i].port ? "IO " : "MEM", i, DIRECT_TO_PHYSICAL(device->bars[i].base));
        }
    }

    if(device->header->class_code == PCI_CLASS_BRIDGE_DEVICE && device->header->subclass_code == PCI_SUBCLASS_HOST_BRIDGE) {
        /*
         * For the host bridge each of the
         * functions is a possible bus
         */
        if(device->header->multifunction) {
            for(int i = 0; i < 8; i++) {
                scan_bus(segment, i);
            }
        }
    }else {
        /*
         * For a normal device each function is a
         * different device
         */
        if(func == 0) {
            for(int i = 1; i < 8; i++) {
                init_device(segment, bus, slot, i);
            }
        }
    }

    /*
     * If this is a pci to pci bridge we should
     * scan that secondary bus
     */
    if(device->header->class_code == PCI_CLASS_BRIDGE_DEVICE && device->header->subclass_code == PCI_SUBCLASS_PCI_TO_PCI_BRIDGE) {
        arrpush(bus_stack, device);
        scan_bus(segment, device->bridge_header->secondary_bus_number);
        arrpop(bus_stack);
    }

    // add the device
    insert_tail_list(&pci_devices, &device->link);
}

static void scan_bus(uint16_t segment, uint8_t bus) {
    for(uint8_t slot = 0; slot < 32; slot++) {
        init_device(segment, bus, slot, 0);
    }
}

void pci_init() {
    debug_log("[*] Iterating PCI devices\n");

    // use AML to figure the bridges
    LAI_CLEANUP_STATE lai_state_t state;
    lai_init_state(&state);

    // with the PNP ids we can find the host bridges
    LAI_CLEANUP_VAR lai_variable_t pci_pnp_id = LAI_VAR_INITIALIZER;
    LAI_CLEANUP_VAR lai_variable_t pcie_pnp_id = LAI_VAR_INITIALIZER;
    lai_eisaid(&pci_pnp_id, "PNP0A03");
    lai_eisaid(&pcie_pnp_id, "PNP0A08");

    /*
     * Iterate objects under \_SB since
     * that should only contain the root bridges
     */
    lai_nsnode_t* sb = lai_resolve_path(NULL, "\\_SB_");
    struct lai_ns_child_iterator iter = LAI_NS_CHILD_ITERATOR_INITIALIZER(sb);
    lai_nsnode_t* node;
    while((node = lai_ns_child_iterate(&iter))) {
        // if the node does not have pci or pcie pnp id continue
        if(lai_check_device_pnp_id(node, &pci_pnp_id, &state) != 0 && lai_check_device_pnp_id(node, &pcie_pnp_id, &state) != 0) continue;

        // get the bus and segment numbers
        // copied from https://github.com/qword-os/lai/blob/master/helpers/pci.c#L211-L231
        LAI_CLEANUP_VAR lai_variable_t bus_number = LAI_VAR_INITIALIZER;
        uint64_t bbn_result = 0;
        lai_nsnode_t *bbn_handle = lai_resolve_path(node, "_BBN");
        if (bbn_handle) {
            if (lai_eval(&bus_number, bbn_handle, &state)) {
                debug_log("[!] failed to evaluate _BBN\n");
                continue;
            }
            lai_obj_get_integer(&bus_number, &bbn_result);
        }

        LAI_CLEANUP_VAR lai_variable_t seg_number = LAI_VAR_INITIALIZER;
        uint64_t seg_result = 0;
        lai_nsnode_t *seg_handle = lai_resolve_path(node, "_SEG");
        if (seg_handle) {
            if (lai_eval(&seg_number, seg_handle, &state)){
                debug_log("[!] failed to evaluate _SEG\n");
                continue;
            }
            lai_obj_get_integer(&seg_number, &seg_result);
        }

        // this will recursively init all other devices as well
        scan_bus(seg_result, bbn_result);
    }

    if(bus_stack != NULL) {
        arrfree(bus_stack);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PCI Utilities
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void* pci_get_config_space(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t function) {
    // iterate the mcfg
    for(size_t i = 0; i < ((acpi_mcfg->header.length - sizeof(acpi_mcfg_t)) / sizeof(mcfg_entry_t)); i++) {
        mcfg_entry_t* entry = &acpi_mcfg->entries[i];

        // check if entry is in the range
        if(entry->segment == seg && entry->start_pci_bus <= bus && entry->end_pci_bus >= bus) {
            // get the base address
            uintptr_t phys = entry->base + (((bus - entry->start_pci_bus) << 20u) | (slot << 15u) | function << 12u);

            // map it if not mapped
            if(!vmm_is_mapped(&kernel_process->vmm_handle, PHYSICAL_TO_DIRECT(phys), PAGE_SIZE)) {
                vmm_map(&kernel_process->vmm_handle, phys, PHYSICAL_TO_DIRECT(phys), PAGE_SIZE, PAGE_SUPERVISOR_READWRITE, DEFAULT_CACHE);
            }

            // return the buffer
            return PHYSICAL_TO_DIRECT((void*)phys);
        }
    }

    // return null if not found
    return NULL;
}
