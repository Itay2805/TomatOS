#include "lapic.h"
#include "apic.h"

#include <drivers/acpi/tables/madt.h>
#include <memory/vmm.h>
#include <buf.h>
#include <drivers/rtc/rtc.h>
#include <interrupts/interrupts.h>
#include <locks/spinlock.h>
#include <cpu/atomic.h>

static char* mmio_base;
uint32_t lapic_timer_ticks_per_second;

/**
 * Will calculate the ticks per second by polling the rtc chip
 * @return
 */
static error_t mesure_ticks_per_second() {
    error_t err = NO_ERROR;
    uint64_t start, end;

    if(lapic_timer_ticks_per_second != 0) goto cleanup;

    // set the timer vector (none-periodic at the start)
    lapic_lvt_t lvt_timer = {
            .vector = 0,
            .periodic = false,
            .delievery_mode = LAPIC_LVT_FIXED
    };
    lapic_write(LAPIC_REG_LVT_TIMER, lvt_timer.raw);
    lapic_write(LAPIC_REG_TIMER_DEVIDER, LAPIC_TIMER_DIVIDER_1); // no divide

    // wait till we reach a second
    CHECK_AND_RETHROW(rtc_unixtime(&start));
    do {
        CHECK_AND_RETHROW(rtc_unixtime(&end));
    } while(end - start != 1);


    // sleep
    CHECK_AND_RETHROW(rtc_unixtime(&start));

    // set to max count
    lapic_write(LAPIC_REG_TIMER_INITIAL_COUNT, 0xFFFFFFFF);
    do {
        CHECK_AND_RETHROW(rtc_unixtime(&end));
    } while(end - start != 1);

    // read out the count
    lapic_timer_ticks_per_second = 0xFFFFFFFF - lapic_read(LAPIC_REG_TIMER_CURRENT_COUNT);

    log_debug("\t\t\tTicks per seconds: %llu", lapic_timer_ticks_per_second);

cleanup:
    return err;
}

static error_t set_lapic_timer() {
    // TODO: Configureable vector
    lapic_lvt_t lvt_timer = {
            .vector = INTERRUPT_IRQ_BASE + INTERRUPT_TIMER,
            .periodic = true,
            .delievery_mode = LAPIC_LVT_FIXED
    };
    lapic_write(LAPIC_REG_LVT_TIMER, lvt_timer.raw);
    lapic_write(LAPIC_REG_TIMER_DEVIDER, LAPIC_TIMER_DIVIDER_1); // no divide
    lapic_write(LAPIC_REG_TIMER_INITIAL_COUNT, lapic_timer_ticks_per_second / 1000);

    return NO_ERROR;
}

static error_t set_nmis() {
    error_t err = NO_ERROR;
    uint64_t procid = apic_get_processor_id();

    for(madt_nmi_t** it = madt_nmis; it < buf_end(madt_nmis); it++) {
        madt_nmi_t* nmi = *it;
        if(nmi->processor_id == 0xFF || nmi->processor_id == procid) {
            lapic_lvt_t lvt = {
                .vector = 0,
                .delievery_mode = LAPIC_LVT_NMI,
                .level_triggered = (uint32_t) (nmi->flags & MADT_FLAG_LEVEL_TRIGGERED),
                .polarity = (uint32_t)(nmi->flags & MADT_FLAG_ACTIVE_LOW),
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

error_t lapic_init() {
    error_t err = NO_ERROR;

    mmio_base = PHYSICAL_ADDRESS(madt->lapic_addr);
    if(!vmm_is_mapped(kernel_address_space, (uintptr_t) mmio_base)) {
        CHECK_AND_RETHROW(vmm_map(kernel_address_space, mmio_base, (void*)(uintptr_t)madt->lapic_addr, PAGE_ATTR_WRITE));
    }

    log_info("\tInitializing Local APIC #%d", lapic_get_id());

    log_debug("\t\tSetting Local APIC timer");
    CHECK_AND_RETHROW(mesure_ticks_per_second());
    CHECK_AND_RETHROW(set_lapic_timer());

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
    uint32_t reg = lapic_read(LAPIC_REG_SIVR);
    lapic_svr_t* svr = (lapic_svr_t *) &reg;
    svr->enabled = true;
    svr->vector = LAPIC_SPURIOUS_VECTOR;
    lapic_write(LAPIC_REG_SIVR, reg);
    return NO_ERROR;
}

error_t lapic_send_eoi() {
    lapic_write(LAPIC_REG_EOI, 0);
    return NO_ERROR;
}

error_t lapic_sleep(uint32_t millis) {
    uint32_t ticks_to_wait = (uint32_t) (millis * (lapic_timer_ticks_per_second / 1000));

    // save the state of the lapic
    lapic_lvt_t old_lvt;
    old_lvt.raw = lapic_read(LAPIC_REG_LVT_TIMER);
    uint32_t initial = lapic_read(LAPIC_REG_TIMER_INITIAL_COUNT);
    uint32_t divider = lapic_read(LAPIC_REG_TIMER_DEVIDER);

    // set a new state for the timer
    lapic_lvt_t timer_lvt = {
        // we don't care about the actual interrupt
        .vector = 0xff,
        .delievery_mode = LAPIC_LVT_FIXED,
        .periodic = false,
        .masked = true,
    };
    lapic_write(LAPIC_REG_LVT_TIMER, timer_lvt.raw);
    lapic_write(LAPIC_REG_TIMER_DEVIDER, LAPIC_TIMER_DIVIDER_1);

    // start the sleep
    lapic_write(LAPIC_REG_TIMER_INITIAL_COUNT, ticks_to_wait);
    while(lapic_read(LAPIC_REG_TIMER_CURRENT_COUNT) > 0) {
        _pause();
    }

    // restore the timer state
    lapic_write(LAPIC_REG_LVT_TIMER, old_lvt.raw);
    lapic_write(LAPIC_REG_TIMER_DEVIDER, divider);
    lapic_write(LAPIC_REG_TIMER_INITIAL_COUNT, initial);
    return NO_ERROR;
}
