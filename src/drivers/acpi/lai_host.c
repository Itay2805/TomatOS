#include <lai/host.h>
#include <stdlib.h>
#include <stdbool.h>
#include <interrupts/interrupts.h>
#include <drivers/portio.h>
#include <drivers/pci/pci.h>
#include "rsdt.h"
#include "fadt.h"
#include "rsdp.h"

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
    log_info(signature);
    if(index == 0) {
        if(SIGNATURE_CHECK(signature, "DSDT")) {
            return dsdt;
        }
        return rsdt_search(signature);
    }else {
        log_warn("doesn't support more than the first index!");
    }
    return NULL;
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

void laihost_pci_write(uint8_t bus, uint8_t function, uint8_t device, uint16_t offset, uint32_t data) {
    pcidev_t dev;
    dev.legacy.bus = bus;
    dev.legacy.function = function;
    dev.legacy.slot = device;

    laihost_panic("laihost_pci_write is stub");
}

uint32_t laihost_pci_read(uint8_t bus, uint8_t function, uint8_t device, uint16_t offset) {
    pcidev_t dev;
    dev.legacy.bus = bus;
    dev.legacy.function = function;
    dev.legacy.slot = device;

    return pci_legacy_config_read_32(&dev, offset);
}

void laihost_sleep(uint64_t duration) {
    laihost_panic("laihost_sleep is stub");
}

void laihost_handle_amldebug(lai_object_t* obj) {}
