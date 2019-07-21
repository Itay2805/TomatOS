#include <acpi/tables/hpet.h>
#include <stb/stb_ds.h>
#include <common/cpu/atomic.h>
#include "hpet.h"

////////////////////////////////////////////
// HPET resource management
////////////////////////////////////////////

static hpet_t* hpets;

static void hpet_write(hpet_t* hpet, size_t offset, uint64_t value) {
    _barrier();
    POKE64(hpet->addr + offset) = value;
}

static uint64_t hpet_read(hpet_t* hpet, size_t offset) {
    _barrier();
    return POKE64(hpet->addr + offset);
}

static void hpet_timer_write(hpet_t* hpet, hpet_timer_t* timer, size_t offset, uint64_t value) {
    _barrier();
    hpet_write(hpet, HPET_TIMER_BASE_OFFSET(timer->id) + offset, value);
}

static uint64_t hpet_timer_read(hpet_t* hpet, hpet_timer_t* timer, size_t offset) {
    _barrier();
    return hpet_read(hpet, HPET_TIMER_BASE_OFFSET(timer->id) + offset);
}

/**
 * This will set the comparator of the given timer and will start the timer
 *
 * @param hpet      [IN]
 * @param timer     [IN]
 * @param millis    [IN]
 */
static error_t hpet_timer_set(hpet_t* hpet, hpet_timer_t* timer, uint64_t millis) {
    // set the interval
    hpet_timer_write(hpet, timer, HPET_REG_TIMER_COMPARATOR_VALUE, MILLIS_TO_FEMTO(millis));

    // set the actual config
    hpet_reg_timer_config_t conf = { .raw = hpet_timer_read(hpet, timer, HPET_REG_TIMER_CONFIGURATION) };
    conf.msi_enable = true;
    conf.type = HPET_TIMER_TYPE_ONE_TIME;
    hpet_timer_write(hpet, timer, HPET_REG_TIMER_CONFIGURATION, conf.raw);
    return NO_ERROR;
}

////////////////////////////////////////////
// Interrupt handler
////////////////////////////////////////////

static error_t handle_timer_interrupt(registers_t* regs) {
    error_t err = NO_ERROR;

    CHECK(arrlen(hpets) >= 1);

    // make sure an interrupt was fired by the HPET
    // for now we assume only interrupt from the first timer of the first hpet
    uint64_t intstatus = hpet_read(&hpets[0], HPET_REG_GENERAL_INTERRUPT_STATUS);
    if((intstatus & (1 << hpets[0].timers[0].id)) == 0) goto cleanup;

    // TODO: Update timers

    // reset
    hpet_timer_set(&hpets[0], 0, 1);

cleanup:
    return err;
}

////////////////////////////////////////////
// API Implementation
////////////////////////////////////////////

error_t hpet_init() {
    error_t err = NO_ERROR;

    // make sure we have the hpet table
    CHECK(hpet_table);
    CHECK_TRACE(hpet_table->addr.address_space_id == ACPI_ADDRESS_SPACE_MMIO,
            "We only support MMIO as access mechanism for HPET currently");

    uint64_t base_addr = hpet_table->addr.address;

    // map it
    if(!vmm_is_mapped(kernel_address_space, CONVERT_TO_DIRECT(base_addr))) {
        CHECK_AND_RETHROW(vmm_map(kernel_address_space,
                (void*)CONVERT_TO_DIRECT((uintptr_t)base_addr),
                (void *)(uintptr_t)base_addr,
                PAGE_ATTR_WRITE));
    }

    // use the hpet table for the first hpet
    hpet_t hpet = {
        .addr = CONVERT_TO_DIRECT(base_addr),
        .timers = NULL
    };

    hpet_reg_general_cap_t hpet_cap = { .raw = hpet_read(&hpet, HPET_REG_GENERAL_CAPS) };
    log_info("Initializing HPET #%d (0x%016p, vendor %04x)", hpet_table->hpet_id, base_addr, hpet_cap.vendor_id);

    log_info("\tHas total of %d timers", hpet_cap.timer_count + 1);

    // find all the timers we are gonna support
    for(int i = 0; i < hpet_cap.timer_count + 1; i++) {
        hpet_timer_t timer = {
                .id = (size_t) i
        };

        hpet_reg_timer_config_t conf = { .raw = hpet_timer_read(&hpet, &timer, HPET_REG_TIMER_CONFIGURATION) };

        if(conf.supports_msi) {
            log_info("\tTimer #%d is supported", i);
            arrpush(hpet.timers, timer);
        }else {
            log_warn("\tTimer #%d does not support MSIs, ignoring", i, hpet_table->hpet_id);
        }
    }
    log_info("\tGot %d good timers", arrlen(hpet.timers));

    CHECK_TRACE(arrlen(hpet.timers) > 0, "Could not find any good timers");

    // set the interrupt route
    hpet.timers[0].vector = interrupt_allocate();
    hpet_reg_timer_msi_config_t route = {
        .address = {
            .destination_id = 0xF,
            .destination_mode = 0,
            .base_address = 0x0FEE
        },
        .data = {
            .vector = hpet.timers[0].vector,
            .delivery_mode = LAPIC_DELIVERY_MODE_LOWEST_PRIORITY,
            .trigger_mode = 1,
            .level = 0
        }
    };
    hpet_timer_write(&hpet, &hpet.timers[0], HPET_REG_TIMER_MSI_CONFIG, route.raw);

    // set the timeout
    hpet_timer_set(&hpets[0], 0, 1);

    // register the interrupt
    interrupt_register(hpet.timers[0].vector, handle_timer_interrupt);
    log_info("Configured Timer %d.%d to 1ms periodic timer", hpet_table->hpet_id, hpet.timers[0].id);

    // enable the HPET
    hpet_reg_general_config_t gen_conf = { .raw = hpet_read(&hpet, HPET_REG_GENERAL_CONFIG) };
    gen_conf.enable = true;
    gen_conf.legacy_replacement = false;
    hpet_write(&hpet, HPET_REG_GENERAL_CONFIG, gen_conf.raw);

    arrpush(hpets, hpet);

cleanup:
    return err;
}

error_t hpet_stall(int64_t millis) {
    error_t err = NO_ERROR;

    CHECK(arrlen(hpets) > 0);

    // use the first hpet
    hpet_t* hpet = &hpets[0];


    size_t need = (size_t) MILLIS_TO_FEMTO(millis);
    size_t start = hpet_read(hpet, HPET_REG_MAIN_COUNTER_VALUE);

    while(hpet_read(hpet, HPET_REG_MAIN_COUNTER_VALUE) - start < need) {
        _pause();
    }

cleanup:
    return err;
}

error_t hpet_find_all() {
    // TODO
    return NO_ERROR;
}

error_t hpet_set_timeout(hpet_timeout_handler_f handler, void* user, uint64_t millis);

