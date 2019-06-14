#include <lai/host.h>
#include <stdlib.h>
#include <stdbool.h>
#include <interrupts/interrupts.h>
#include <drivers/portio.h>
#include <drivers/pci/pci.h>
#include <drivers/apic/lapic.h>
#include <lai/core.h>
#include <string.h>

#include "tables/rsdt.h"
#include "tables/fadt.h"
#include "tables/rsdp.h"

#define SIGNATURE_CHECK(str1, str2) (str1[0] == str2[0] && str1[1] == str2[1] && str1[2] == str2[2] && str1[3] == str2[3])

void* laihost_malloc(size_t size) {
    return malloc(size);
}

void* laihost_realloc(void* ptr, size_t size) {
    return realloc(ptr, size);
}

void laihost_free(void* ptr) {
    return free(ptr);
}

static char buffer[1024];

void laihost_log(int lvl, const char* str) {
    strcpy(buffer, str);
    buffer[strlen(buffer) - 1] = 0;
    switch(lvl) {
        case LAI_DEBUG_LOG: log_debug(buffer); break;
        case LAI_WARN_LOG: log_warn(buffer); break;
        default: log_info(buffer); break;
    }
}

void laihost_panic(const char* str) {
    strcpy(buffer, str);
    buffer[strlen(buffer) - 1] = 0;
    log_critical(buffer);
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

//void laihost_pci_write(uint8_t bus, uint8_t function, uint8_t device, uint16_t offset, uint32_t data) {
//    pcidev_t dev;
//    dev.legacy.bus = bus;
//    dev.legacy.function = function;
//    dev.legacy.slot = device;
//
//    laihost_panic("laihost_pci_write is stub");
//}

uint32_t laihost_pci_read(uint8_t bus, uint8_t function, uint8_t device, uint16_t offset) {
    pcidev_t dev;
    dev.legacy.bus = bus;
    dev.legacy.function = function;
    dev.legacy.slot = device;

    return pci_legacy_config_read_32(&dev, offset);
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
