#include "lapic.h"

#include <drivers/acpi/madt.h>
#include <memory/vmm.h>
#include <buf.h>
#include <drivers/rtc/rtc.h>
#include <interrupts/interrupts.h>

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
            .vector = INTERRUPT_TIMER,
            .periodic = false,
            .delievery_mode = LAPIC_LVT_FIXED
    };
    lapic_write(LAPIC_REG_LVT_TIMER, *(uint32_t*)&lvt_timer);
    lapic_write(LAPIC_REG_TIMER_DEVIDER, 0b111); // no divide

    // wait till we reach a second
    CHECK_AND_RETHROW(rtc_unixtime(&start));
    do {
        CHECK_AND_RETHROW(rtc_unixtime(&end));
    } while(end - start == 1);

    // set to max count
    lapic_write(LAPIC_REG_TIMER_INITIAL_COUNT, 0xFFFFFFFF);

    // sleep
    CHECK_AND_RETHROW(rtc_unixtime(&start));
    do {
        CHECK_AND_RETHROW(rtc_unixtime(&end));
    } while(end - start == 1);

    // read out the count
    lapic_timer_ticks_per_second = 0xFFFFFFFF - lapic_read(LAPIC_REG_TIMER_CURRENT_COUNT);

    log_debug("\t\t\tTicks per seconds: %llu", lapic_timer_ticks_per_second);

cleanup:
    return err;
}

static error_t set_lapic_timer() {
    lapic_lvt_t lvt_timer = {
            .vector = INTERRUPT_TIMER,
            .periodic = true,
            .delievery_mode = LAPIC_LVT_FIXED
    };
    lapic_write(LAPIC_REG_LVT_TIMER, *(uint32_t*)&lvt_timer);
    lapic_write(LAPIC_REG_TIMER_DEVIDER, 0b111); // no divide
    lapic_write(LAPIC_REG_TIMER_INITIAL_COUNT, lapic_timer_ticks_per_second / 100);

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