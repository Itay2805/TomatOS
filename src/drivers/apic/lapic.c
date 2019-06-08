#include "lapic.h"

#include <drivers/acpi/madt.h>
#include <memory/vmm.h>
#include <buf.h>

static char* mmio_base;

error_t lapic_init() {
    error_t err = NO_ERROR;

    mmio_base = PHYSICAL_ADDRESS(madt->lapic_addr);
    if(!vmm_is_mapped(kernel_address_space, (uintptr_t) mmio_base)) {
        CHECK_AND_RETHROW(vmm_map(kernel_address_space, mmio_base, (void*)(uintptr_t)madt->lapic_addr, PAGE_ATTR_WRITE));
    }

    log_info("\tInitializing Local APIC #%d", lapic_get_id());

    CHECK_AND_RETHROW(lapic_enable());

cleanup:
    return err;
}

error_t lapic_eoi() {
    lapic_write(LAPIC_REG_EOI, 0);
    return NO_ERROR;
}

void lapic_write(uint32_t reg, uint32_t value) {
    *(uint32_t*)&mmio_base[reg] = value;
}

uint32_t lapic_read(uint32_t reg) {
    return *(uint32_t*)&mmio_base[reg];
}

uint8_t lapic_get_id() {
    return (uint8_t) (lapic_read(LAPIC_REG_ID) >> 24 & 0xFF);
}

error_t lapic_enable() {
    uint32_t reg = lapic_read(LAPIC_REG_SIVR);
    lapic_svr_t* svr = (lapic_svr_t *) &reg;
    svr->enabled = true;
    svr->vector = LAPIC_SPURIOUS_VECTOR;
    lapic_write(LAPIC_REG_SIVR, reg);
    return NO_ERROR;
}