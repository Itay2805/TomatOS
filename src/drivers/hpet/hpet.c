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
    if(offset % 8 != 0) {
        log_error("Attempted to do unaligned hpet access");
        asm("int $0x11");
    }
    hpet->mmio[offset / 8] = value;
}

static uint64_t hpet_read(hpet_t* hpet, size_t offset) {
    _barrier();
    if(offset % 8 != 0) {
        log_error("Attempted to do unaligned hpet access");
        asm("int $0x11");
    }
    return hpet->mmio[offset / 8];
}

static void hpet_timer_write(hpet_t* hpet, hpet_timer_t* timer, size_t offset, uint64_t value) {
    _barrier();
    hpet_write(hpet, HPET_TIMER_BASE_OFFSET(timer->id) + offset, value);
}

static uint64_t hpet_timer_read(hpet_t* hpet, hpet_timer_t* timer, size_t offset) {
    _barrier();
    return hpet_read(hpet, HPET_TIMER_BASE_OFFSET(timer->id) + offset);
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
    CHECK_AND_RETHROW(vmm_map_direct(base_addr, KB(4)));

    // use the hpet table for the first hpet
    hpet_t hpet = {
        .mmio = (uint64_t*)CONVERT_TO_DIRECT(base_addr),
        .timers = NULL
    };

    // get the caps and save the femto per ticks
    hpet_reg_general_cap_t hpet_cap = { .raw = hpet_read(&hpet, HPET_REG_GENERAL_CAPS) };
    hpet.femto_per_tick = hpet_cap.femto_per_Tick;
    log_info("Initializing HPET #%d (0x%016p, vendor %04x)", hpet_table->hpet_id, base_addr, hpet_cap.vendor_id);

    // enable the HPET
    hpet_reg_general_config_t gen_conf = { .raw = hpet_read(&hpet, HPET_REG_GENERAL_CONFIG) };
    gen_conf.enable = true;
    gen_conf.legacy_replacement = false;
    hpet_write(&hpet, HPET_REG_GENERAL_CONFIG, gen_conf.raw);

    // add it
    arrpush(hpets, hpet);

cleanup:
    return err;
}

error_t hpet_stall(int64_t millis) {
    error_t err = NO_ERROR;

    CHECK(arrlen(hpets) > 0);

    // use the first hpet
    hpet_t* hpet = &hpets[0];

    size_t need = (size_t) MILLIS_TO_FEMTO(millis) / hpet->femto_per_tick;
    size_t start = hpet_read(hpet, HPET_REG_MAIN_COUNTER_VALUE);
    while(hpet_read(hpet, HPET_REG_MAIN_COUNTER_VALUE) - start < need) {
        _pause();
    }

cleanup:
    return err;
}

error_t hpet_find_all() {
    // TODO: using lai
    log_warn("hpet_find_all not implemented yet");
    return NO_ERROR;
}

error_t hpet_set_timeout(hpet_timeout_handler_f handler, void* user, uint64_t millis) {
    return ERROR_NOT_IMPLEMENTED;
}

uint64_t hpet_get_millis() {
    // use the first hpet
    hpet_t* hpet = &hpets[0];
    return FEMTO_TO_MILLIS(hpet_read(hpet, HPET_REG_MAIN_COUNTER_VALUE) * hpet->femto_per_tick);
}
