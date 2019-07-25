#include <lai/host.h>
#include <stdlib.h>
#include <logger/logger.h>
#include <error.h>
#include <interrupts/interrupts.h>
#include <drivers/hpet/hpet.h>
#include <pci/pci.h>
#include <drivers/portio.h>
#include <string.h>
#include <acpi/tables/rsdt.h>
#include <acpi/tables/fadt.h>

void* laihost_malloc(size_t size) {
    return kmalloc(size);
}

void* laihost_realloc(void* ptr, size_t size) {
    return realloc(ptr, size);
}

void laihost_free(void* ptr) {
    return kfree(ptr);
}

void laihost_log(int level, const char* str) {
    switch(level) {
        case LAI_DEBUG_LOG: log_debug(str); break;
        case LAI_WARN_LOG: log_warn(str); break;
        default: log_info(str); break;
    }
}

void laihost_panic(const char* str) {
    log_critical(str);

    _cli(); while(true) _hlt();
}

void* laihost_scan(char* signature, size_t index) {
    if(strncmp(signature, "DSDT", 4) == 0) {
        return dsdt;
    }else {
        return rsdt_search(signature, (int) index);
    }
}

void* laihost_map(size_t addr, size_t size) {
    error_t err = NO_ERROR;

    for(uintptr_t a = ALIGN_PAGE_DOWN(addr); a < ALIGN_PAGE_UP(addr + size); a += KB(4)) {
        if(!vmm_is_mapped(kernel_address_space, CONVERT_TO_DIRECT(a))) {
            CHECK_AND_RETHROW(vmm_map(kernel_address_space, (void *) CONVERT_TO_DIRECT(a), (void *) a, PAGE_ATTR_WRITE));
        }
    }

cleanup:
    if(err != NO_ERROR) {
        _cli(); while(true) _hlt();
    }

    return (void *) CONVERT_TO_DIRECT(addr);
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

void laihost_pci_writeb(uint16_t segment, uint8_t bus, uint8_t device, uint8_t function, uint16_t offset, uint8_t value) {
    error_t err = NO_ERROR;

    char* mmio = NULL;
    CHECK_AND_RETHROW(pci_get_and_map_mmio(segment, bus, device, function, &mmio));
    POKE8(mmio + offset) = value;

cleanup:
    if(err != NO_ERROR) {
        CATCH(err);
        _cli(); while(true) _hlt();
    }
}

uint8_t laihost_pci_readb(uint16_t segment, uint8_t bus, uint8_t device, uint8_t function, uint16_t offset) {
    error_t err = NO_ERROR;

    char* mmio = NULL;
    uint8_t value = 0;
    CHECK_AND_RETHROW(pci_get_and_map_mmio(segment, bus, device, function, &mmio));
    value = POKE8(mmio + offset);

cleanup:
    if(err != NO_ERROR) {
        CATCH(err);
        _cli(); while(true) _hlt();
    }

    return value;
}

void laihost_pci_writew(uint16_t segment, uint8_t bus, uint8_t device, uint8_t function, uint16_t offset, uint16_t value) {
    error_t err = NO_ERROR;

    char* mmio = NULL;
    CHECK_AND_RETHROW(pci_get_and_map_mmio(segment, bus, device, function, &mmio));
    POKE16(mmio + offset) = value;

cleanup:
    if(err != NO_ERROR) {
        CATCH(err);
        _cli(); while(true) _hlt();
    }
}

uint16_t laihost_pci_readw(uint16_t segment, uint8_t bus, uint8_t device, uint8_t function, uint16_t offset) {
    error_t err = NO_ERROR;

    char* mmio = NULL;
    uint16_t value = 0;
    CHECK_AND_RETHROW(pci_get_and_map_mmio(segment, bus, device, function, &mmio));
    value = POKE16(mmio + offset);

cleanup:
    if(err != NO_ERROR) {
        CATCH(err);
        _cli(); while(true) _hlt();
    }

    return value;
}

void laihost_pci_writed(uint16_t segment, uint8_t bus, uint8_t device, uint8_t function, uint16_t offset, uint32_t value) {
    error_t err = NO_ERROR;

    char* mmio = NULL;
    CHECK_AND_RETHROW(pci_get_and_map_mmio(segment, bus, device, function, &mmio));
    POKE32(mmio + offset) = value;

cleanup:
    if(err != NO_ERROR) {
        CATCH(err);
        _cli(); while(true) _hlt();
    }
}

uint32_t laihost_pci_readd(uint16_t segment, uint8_t bus, uint8_t device, uint8_t function, uint16_t offset) {
    error_t err = NO_ERROR;

    char* mmio = NULL;
    uint32_t value = 0;
    CHECK_AND_RETHROW(pci_get_and_map_mmio(segment, bus, device, function, (void*)&mmio));
    value = POKE32(mmio + offset);

cleanup:
    if(err != NO_ERROR) {
        CATCH(err);
        _cli(); while(true) _hlt();
    }

    return value;
}

void laihost_sleep(uint64_t sleep) {
    error_t err = NO_ERROR;

    // TODO: Don't use stall
    CHECK_AND_RETHROW(hpet_stall(sleep));

cleanup:
    if(err != NO_ERROR) {
        _cli(); while(true) _hlt();
    }

}