#include "lapic.h"
#include "apic.h"

#include <drivers/rtc/rtc.h>
#include <interrupts/interrupts.h>
#include <locks/spinlock.h>
#include <acpi/tables/madt.h>
#include <cpu/atomic.h>
#include <memory/vmm.h>
#include <stb/stb_ds.h>
#include <drivers/hpet/hpet.h>

static char* mmio_base;
static uint8_t timer_vector;

static error_t set_nmis() {
    error_t err = NO_ERROR;
    uint64_t procid = apic_get_processor_id();

    for(madt_nmi_t** it = madt_nmis; it < madt_nmis + arrlen(madt_nmis); it++) {
        madt_nmi_t* nmi = *it;
        if(nmi->processor_id == 0xFF || nmi->processor_id == procid) {
            lapic_lvt_lint_t lvt = {
                .vector = 0,
                .delivery_mode = LAPIC_DELIVERY_MODE_NMI,
                .trigger_mode = (uint32_t) (nmi->flags & MADT_FLAG_LEVEL_TRIGGERED),
                .input_pin_polarity = (uint32_t)(nmi->flags & MADT_FLAG_ACTIVE_LOW),
            };

            switch(nmi->lint) {
                case 0: lapic_write(LAPIC_REG_LVT_LINT0, lvt.raw); break;
                case 1: lapic_write(LAPIC_REG_LVT_LINT1, lvt.raw); break;
                default:
                    CHECK_FAIL_ERROR_TRACE(ERROR_NOT_SUPPORTED, "NMI had invalid lint (%d)", nmi->lint);
            }
        }
    }

cleanup:
    return NO_ERROR;
}

static error_t calibrate_timer() {
    error_t err = NO_ERROR;

    // mask the interrupt
    lapic_lvt_timer_t timer = {
            .vector = timer_vector,
            .mask = true,
            .timer_mode = LAPIC_TIMER_MODE_ONE_SHOT
    };
    lapic_write(LAPIC_REG_LVT_TIMER, timer.raw);

    // set the counter to highest possible and stall
    lapic_write(LAPIC_REG_TIMER_DEVIDER, LAPIC_TIMER_DIVIDER_1);
    lapic_write(LAPIC_REG_TIMER_INITIAL_COUNT, UINT32_MAX);

    // stall
    CHECK_AND_RETHROW(hpet_stall(1));

    // read the current count
    uint64_t ticks = UINT32_MAX - lapic_read(LAPIC_REG_TIMER_CURRENT_COUNT);

    // set the initial count to the ticks and enable it
    lapic_write(LAPIC_REG_TIMER_INITIAL_COUNT, (uint32_t) ticks);

    // enable interrupt
    timer.mask = false;
    timer.timer_mode = LAPIC_TIMER_MODE_PERIODIC;
    lapic_write(LAPIC_REG_LVT_TIMER, timer.raw);

cleanup:
    return err;
}

error_t lapic_init() {
    error_t err = NO_ERROR;

    mmio_base = (char*)CONVERT_TO_DIRECT((uintptr_t)madt->lapic_addr);
    if(!vmm_is_mapped(kernel_address_space, (uintptr_t) mmio_base)) {
        CHECK_AND_RETHROW(vmm_map(kernel_address_space, mmio_base, (void*)(uintptr_t)madt->lapic_addr, PAGE_ATTR_WRITE));
    }

    log_info("\tInitializing Local APIC #%d", lapic_get_id());

    timer_vector = interrupt_allocate();
    log_debug("\t\tSetting timer for 1ms (Vector #%d)", timer_vector);
    CHECK_AND_RETHROW(calibrate_timer());

    log_debug("\t\tSetting NMIs");
    CHECK_AND_RETHROW(set_nmis());

    log_debug("\t\tSetting SPR");
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
    lapic_svr_t svr = {.raw = lapic_read(LAPIC_REG_SIVR)};
    svr.software_enable = true;
    svr.spurious_vector = LAPIC_SPURIOUS_VECTOR;
    lapic_write(LAPIC_REG_SIVR, svr.raw);
    return NO_ERROR;
}

error_t lapic_send_eoi() {
    lapic_write(LAPIC_REG_EOI, 0);
    return NO_ERROR;
}
