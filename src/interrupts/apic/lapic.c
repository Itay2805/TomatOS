#include <util/poke.h>
#include <acpi/tables/madt.h>
#include <libc/stddef.h>
#include <memory/vmm.h>
#include <util/stall.h>
#include "lapic.h"
#include "apic.h"

static volatile char* mmio_base;
static uint64_t ticks_per_milli;

static error_t set_nmis() {
    error_t err = NO_ERROR;
    uint64_t procid = apic_get_processor_id();

    for(int i = 0; ; i++) {
        madt_nmi_t* nmi = madt_get_next(MADT_TYPE_NMI, i);
        if(nmi == NULL) break;

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
                default: CHECK_FAILED();
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
            .mask = true,
            .timer_mode = LAPIC_TIMER_MODE_ONE_SHOT
    };
    lapic_write(LAPIC_REG_LVT_TIMER, timer.raw);

    // set the counter to highest possible and stall
    lapic_write(LAPIC_REG_TIMER_DEVIDER, LAPIC_TIMER_DIVIDER_1);
    lapic_write(LAPIC_REG_TIMER_INITIAL_COUNT, UINT32_MAX);

    // stall
    stall(50);

    // read the current count
    ticks_per_milli = (UINT32_MAX - lapic_read(LAPIC_REG_TIMER_CURRENT_COUNT)) / 50;

    return err;
}

error_t lapic_init() {
    error_t err = NO_ERROR;

    mmio_base = (char*)PHYSICAL_TO_DIRECT((uintptr_t)madt_table->lapic_addr);
    if(!vmm_is_mapped(&kernel_handle, (uintptr_t)mmio_base, PAGE_SIZE)) {
        vmm_map(&kernel_handle, madt_table->lapic_addr, (uintptr_t)mmio_base, PAGE_SIZE, PAGE_SUPERVISOR_READWRITE, DEFAULT_CACHE);
    }

    debug_log("[*] \tInitializing Local APIC #%d\n", lapic_get_id());

    // only calibrate once
    if(ticks_per_milli == 0) {
        CHECK_AND_RETHROW(calibrate_timer());
    }

    CHECK_AND_RETHROW(set_nmis());
    CHECK_AND_RETHROW(lapic_enable());

cleanup:
    return err;
}

error_t lapic_eoi() {
    lapic_write(LAPIC_REG_EOI, 0);
    return NO_ERROR;
}

void lapic_write(uint32_t reg, uint32_t value) {
    POKE32(mmio_base + reg) = value;
}

uint32_t lapic_read(uint32_t reg) {
    return POKE32(mmio_base + reg);
}

uint8_t lapic_get_id() {
    if(mmio_base == NULL) {
        return (uint8_t) -1;
    }
    return (uint8_t) (lapic_read(LAPIC_REG_ID) >> 24 & 0xFF);
}

error_t lapic_enable() {
    lapic_svr_t svr = {.raw = lapic_read(LAPIC_REG_SIVR)};
    svr.software_enable = true;
    svr.spurious_vector = LAPIC_SPURIOUS_VECTOR;
    lapic_write(LAPIC_REG_SIVR, svr.raw);
    return NO_ERROR;
}

void lapic_send_eoi() {
    lapic_write(LAPIC_REG_EOI, 0);
}

error_t lapic_set_timer(uint32_t millis, uint8_t vector) {
    error_t err = NO_ERROR;

    CHECK(mmio_base != NULL);

    lapic_lvt_timer_t timer = {
            .vector = vector,
            .mask = false,
            .timer_mode = LAPIC_TIMER_MODE_PERIODIC,
    };

    lapic_write(LAPIC_REG_TIMER_INITIAL_COUNT, (uint32_t) (millis * ticks_per_milli));
    lapic_write(LAPIC_REG_TIMER_DEVIDER, LAPIC_TIMER_DIVIDER_1);
    lapic_write(LAPIC_REG_LVT_TIMER, timer.raw);

cleanup:
    return err;
}

bool lapic_timer_fired() {
    lapic_lvt_timer_t timer = { .raw = lapic_read(LAPIC_REG_LVT_TIMER) };
    return timer.delivery_status == 1;
}

void lapic_send_ipi(uint32_t lapic_id, uint8_t vector) {
    // prepare the icr
    lapic_icr_t icr = (lapic_icr_t){
            .delivery_mode = LAPIC_DELIVERY_MODE_FIXED,
            .vector = vector,
            .destination = lapic_id,
    };

    // write it
    lapic_write(LAPIC_REG_ICR1, icr.raw_high);
    lapic_write(LAPIC_REG_ICR0, icr.raw_low);
}

void lapic_send_ipi_all_excluding_self(uint8_t vector) {
    // prepare the icr
    lapic_icr_t icr = {
            .delivery_mode = LAPIC_DELIVERY_MODE_FIXED,
            .level = 1,
            .vector = vector,
            .destination_shothand = LAPIC_DESTINATION_SHORTHAND_ALL_EXCLUDING_SELF
    };

    // write it
    lapic_write(LAPIC_REG_ICR1, icr.raw_high);
    lapic_write(LAPIC_REG_ICR0, icr.raw_low);
}

void lapic_send_ipi_all_including_self(uint8_t vector) {
    // let everything finish before trying to send an ipi
    memory_fence();

    lapic_icr_t icr = {
            .delivery_mode = LAPIC_DELIVERY_MODE_FIXED,
            .level = 1,
            .vector = vector,
            .destination_shothand = LAPIC_DESTINATION_SHORTHAND_ALL_INCLUDING_SELF
    };

    // write it
    lapic_write(LAPIC_REG_ICR1, icr.raw_high);
    lapic_write(LAPIC_REG_ICR0, icr.raw_low);
}

void lapic_send_init(uint32_t lapic_id) {
    // prepare the icr
    lapic_icr_t icr = {
        .delivery_mode = LAPIC_DELIVERY_MODE_INIT,
        .level = 1,
        .destination = lapic_id,
    };

    // write it
    lapic_write(LAPIC_REG_ICR1, icr.raw_high);
    lapic_write(LAPIC_REG_ICR0, icr.raw_low);
}

void lapic_send_sipi(uint32_t lapic_id, uint32_t entry) {
    // make sure the entry is okie
    ASSERT(entry < 0x100000);
    ASSERT((entry & 0xfff) == 0);

    // prepare the icr
    lapic_icr_t icr = {
        .delivery_mode = LAPIC_DELIVERY_MODE_STARTUP,
        .level = 1,
        .vector = entry >> 12,
        .destination = lapic_id,
    };

    // write it
    lapic_write(LAPIC_REG_ICR1, icr.raw_high);
    lapic_write(LAPIC_REG_ICR0, icr.raw_low);
}
