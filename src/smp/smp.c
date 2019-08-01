#include <common/locks/event.h>
#include <interrupts/interrupts.h>
#include <memory/gdt.h>
#include <interrupts/apic/lapic.h>
#include <drivers/hpet/hpet.h>
#include <acpi/tables/madt.h>
#include <stb/stb_ds.h>
#include "smp.h"
#include "cpustorage.h"

#define SMP_INFO_ADDRESS            0x0500
#define SMP_TRAMPOLINE_ADDRESS      0x1000

typedef struct smp_trampoline_info {
    char flag;
    uint32_t cr3;
    int current_core;
} smp_trampoline_info_t;

static volatile smp_trampoline_info_t* smp_info = 0;
static event_t finished_init = {{0}, 0};

/**
 * Do a per cpu initialization
 *
 * This will setup the storage, the gdt/tss and the lapic for being
 * ready for doing scheduling.
 */
static void per_cpu_init() {
    error_t err = NO_ERROR;

    log_info("Initing cpu #%d", smp_info->current_core);
    CHECK_AND_RETHROW(set_cpu_storage(smp_info->current_core));
    CHECK_AND_RETHROW(per_cpu_gdt_and_tss_init());
    CHECK_AND_RETHROW(lapic_init());

    // unlock the core
    event_signal(&finished_init);

cleanup:
    if(err != NO_ERROR) {
        log_critical("Error during kernel per core initialization (%d) :(", smp_info->current_core);
        _cli(); while(true) _hlt();
    }

    if(smp_info->current_core == 0) {
        return;
    }

    // wait
    while(true) _hlt();
}

error_t smp_init() {
    error_t err = NO_ERROR;

    CHECK_AND_RETHROW(per_cpu_storage_init());

    // SMP info is located at 0x510
    // SMP trampoline is located
    smp_info = (smp_trampoline_info_t*)CONVERT_TO_DIRECT(SMP_INFO_ADDRESS);

    // setup the core processor
    smp_info->current_core = 0;
    per_cpu_init();

    // iterate each of the lapics and boot
    for(int i = 0; i < arrlen(madt_lapics); i++) {
        madt_lapic_t* lapic = madt_lapics[i];

        // ignore boot processor or disabled lapics
        if(!lapic->processor_enabled || lapic->id == lapic_get_id()) {
            continue;
        }

        // we are going to init this cpu!
        smp_info->current_core++;

        // lock the boot lock, the processor will finish it once
        log_warn("TODO: actually init and startup other cores");
        continue;

        // send init
        lapic_send_init(lapic->id);
        hpet_stall(10);

        // now send startup and wait for flag
        lapic_send_sipi(lapic->id, SMP_TRAMPOLINE_ADDRESS);
        hpet_stall(1);
        if(!smp_info->flag) {
            // failed first, send second
            lapic_send_sipi(lapic->id, SMP_TRAMPOLINE_ADDRESS);
            hpet_stall(1000);
            CHECK_TRACE(smp_info->flag, "Failed to init startup processor");
        }

        // now wait for the full init of the core
        // with 1 second timeout
        CHECK_TRACE(event_wait(&finished_init, 1000), "Got timeout when waiting for finish init event!");
    }

cleanup:
    return err;
}
