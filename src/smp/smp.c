#include <memory/gdt.h>
#include <interrupts/apic/lapic.h>
#include "smp.h"
#include "percpustorage.h"

error_t smp_init() {
    error_t err = NO_ERROR;

    // BSP init
    CHECK_AND_RETHROW(per_cpu_storage_init());
    CHECK_AND_RETHROW(lapic_init());
    CHECK_AND_RETHROW(per_cpu_gdt_and_tss_init());

cleanup:
    return err;
}
