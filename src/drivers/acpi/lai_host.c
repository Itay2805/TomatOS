#include <lai/host.h>
#include <stdlib.h>
#include <stdbool.h>
#include <interrupts/interrupts.h>
#include <drivers/portio.h>
#include <drivers/pci/pci.h>
#include <drivers/apic/lapic.h>
#include <lai/core.h>
#include <string.h>
#include <memory/mm.h>
#include <drivers/pci/legacy.h>
#include <buf.h>

#include "tables/rsdt.h"
#include "tables/fadt.h"
#include "tables/rsdp.h"

#define SIGNATURE_CHECK(str1, str2) (str1[0] == str2[0] && str1[1] == str2[1] && str1[2] == str2[2] && str1[3] == str2[3])

void* laihost_malloc(size_t size) {
    error_t err = NO_ERROR;
    void* out_ptr;

    if(size == 0) {
        log_warn("lai attempts to allocate 0 bytes, allocating 1 instead");
        size = 1;
    }

    CHECK_AND_RETHROW(mm_allocate_aligned(size, 8, &out_ptr));

cleanup:
    if(err) {
        laihost_panic("laihost_malloc failed");
    }
    return out_ptr;
}

void* laihost_realloc(void* ptr, size_t size) {
    error_t err = NO_ERROR;
    void* tmp = ptr;

    if(size == 0) {
        log_warn("lai attempts to allocate 0 bytes, allocating 1 instead");
        size = 1;
    }

    CHECK_AND_RETHROW(mm_reallocate(&tmp, size));

cleanup:
    if(err) {
        laihost_panic("laihost_realloc failed");
    }

    return tmp;
}

void laihost_free(void* ptr) {
    error_t err = NO_ERROR;

    CHECK_AND_RETHROW(mm_free(ptr));

cleanup:
    if(err) {
        laihost_panic("laihost_free failed");
    }
}

void laihost_log(int lvl, const char* str) {
    switch(lvl) {
        case LAI_DEBUG_LOG: log_debug(str); break;
        case LAI_WARN_LOG: log_warn(str); break;
        default: log_info(str); break;
    }
}

void laihost_panic(const char* str) {
    log_critical(str);
    log_critical("Halting kernel :(");
    _cli();
    while(true) {
        _hlt();
    }
}

void* laihost_scan(char* signature, size_t index) {
    if(SIGNATURE_CHECK(signature, "DSDT")) {
        return dsdt;
    }
    return rsdt_search(signature, (int) index);
}

void* laihost_map(size_t phys_addr, size_t count) {
    // make sure it is all mapped
    for(uintptr_t addr = ALIGN_DOWN(phys_addr, KB(4)); addr < ALIGN_UP(phys_addr + count * KB(4), KB(4)); addr += KB(4)) {
        if(!vmm_is_mapped(kernel_address_space, (uintptr_t) PHYSICAL_ADDRESS(addr))) {
            vmm_map(kernel_address_space, PHYSICAL_ADDRESS(addr), (void *) addr, PAGE_ATTR_WRITE);
        }
    }
    return PHYSICAL_ADDRESS(phys_addr);
}

void laihost_outb(uint16_t port, uint8_t value) {
    outb(port, value);
}

void laihost_outw(uint16_t port, uint16_t value) {
    outw(port, value);
}

void laihost_outd(uint16_t port, uint32_t value) {
    outl(port, value);
}

uint8_t laihost_inb(uint16_t port) {
    return inb(port);
}

uint16_t laihost_inw(uint16_t port) {
    return inw(port);
}

uint32_t laihost_ind(uint16_t port) {
    return inl(port);
}

void laihost_pci_write(uint8_t bus, uint8_t device, uint8_t function, uint16_t offset, uint32_t data) {
    error_t err = NO_ERROR;

    // find the device
    pcidev_t* dev = NULL;
    for(pcidev_t* it = pcidevs; it < buf_end(pcidevs); it++) {
        if(it->bus == bus && it->function == function && it->device == device && it->segment == 0) {
            dev = it;
        }
    }

    // do the write
    CHECK_ERROR(dev, ERROR_NOT_FOUND);
    pci_config_write_32(dev, offset, data);

cleanup:
    if(err != NO_ERROR) {
        log_warn("not found 0.%x.%x.%x, falling back on direct legacy pci for laihost_pci_write", bus, device, function);
        pcidev_t pcidev = {
                .function = function,
                .device = device,
                .bus = bus,
                .segment = 0,
        };
        pci_legacy_config_write_32(&pcidev, offset, data);
    }
}

uint32_t laihost_pci_read(uint8_t bus, uint8_t device, uint8_t function, uint16_t offset) {
    error_t err = NO_ERROR;

    // find the device
    pcidev_t* dev = NULL;
    for(pcidev_t* it = pcidevs; it < buf_end(pcidevs); it++) {
        if(it->segment == 0 && it->bus == bus && it->device == device && it->function == function) {
            dev = it;
        }
    }

    // do the read
    CHECK_ERROR(dev, ERROR_NOT_FOUND);
    uint32_t res = pci_config_read_32(dev, offset);

cleanup:
    if(err != NO_ERROR) {
        log_warn("not found 0.%x.%x.%x, falling back on direct legacy pci for laihost_pci_read", bus, device, function);
        pcidev_t pcidev = {
            .function = function,
            .device = device,
            .bus = bus,
            .segment = 0,
        };
        res = pci_legacy_config_read_32(&pcidev, offset);
    }
    return res;
}

void laihost_sleep(uint64_t duration) {
    while(duration / UINT32_MAX > 0) {
        lapic_sleep(UINT32_MAX);
        duration -= UINT32_MAX;
    }

    // sleep the reminder
    if(duration % UINT32_MAX != 0) {
        lapic_sleep((uint32_t)(duration % UINT32_MAX));
    }
}

void laihost_handle_amldebug(lai_object_t* obj) {
    switch(obj->type) {
        case LAI_INTEGER: {
            log_debug("amldebug: %lu", obj->integer);
        } break;
        case LAI_STRING: {
            log_debug("amldebug: %s", obj->string);
        } break;
        default:break;
    }
}
