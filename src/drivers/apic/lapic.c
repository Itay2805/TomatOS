#include "lapic.h"

#include <drivers/acpi/madt.h>
#include <memory/vmm.h>
#include <buf.h>

static char* mmio_base;
static int nmi_base = 0xff - 1;

error_t lapic_init() {
    error_t err = NO_ERROR;

    log_info("\tInitializing Local APIC");
    mmio_base = PHYSICAL_ADDRESS(madt->lapic_addr);
    if(!vmm_is_mapped(kernel_address_space, (uintptr_t) mmio_base)) {
        CHECK_AND_RETHROW(vmm_map(kernel_address_space, mmio_base, (void*)(uintptr_t)madt->lapic_addr, PAGE_ATTR_WRITE));
    }

    // first install the NMIs
    if(buf_len(madt_nmis) != 0) {
        nmi_base -= buf_len(madt_nmis);
        log_debug("\t\tSetting NMIs (base=0x%02x)", nmi_base);
        for(int i = 0; i < buf_len(madt_nmis); i++) {
            madt_nmi_t* nmi = madt_nmis[i];

            // NMI #num: vector -> lint
            log_debug("\t\t\tNMI #%d: 0x%02x -> 0x%02x", i, (nmi_base + i), nmi->lint);

            // set the registers
            lapic_lvtr_t vtr = {
                .vector =           (uint8_t) (nmi_base + i),
                .nmi =              true,
                .low_triggered =    (uint8_t) ((nmi->flags & MADT_FLAG_ACTIVE_LOW) != 0),
                .level_triggered =  (uint8_t) ((nmi->flags & MADT_FLAG_LEVEL_TRIGGERED) != 0),
            };

            // write them out
            switch(nmi->lint) {
                case 0: lapic_write(LAPIC_REG_LVTR_LINT0, vtr.raw); break;
                case 1: lapic_write(LAPIC_REG_LVTR_LINT1, vtr.raw); break;
                default: {
                    CHECK_FAIL_ERROR_TRACE(ERROR_CHECK_FAILED, "NMI has invalid lint (%d)", nmi->lint);
                } break;
            }
        }
    }else {
        log_debug("\t\tNo NMIs found, ignoring");
    }

    // now enable lapic by setting the sivr
    log_debug("\t\tSetting SIVR");
    lapic_write(LAPIC_REG_SIVR, lapic_read(LAPIC_REG_SIVR) | 0x1ff);

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
