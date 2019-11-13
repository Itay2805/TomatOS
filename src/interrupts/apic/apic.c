#include <acpi/tables/madt.h>
#include <util/debug.h>

#include <stddef.h>
#include <util/arch.h>
#include <memory/vmm.h>

#include "apic.h"
#include "ioapic.h"
#include "lapic.h"

static uint32_t lapic_pid_map[255];

void apic_init() {

    debug_log("[*] Preparing APIC\n");
    ASSERT(acpi_madt != NULL);

    // TODO: disable legacy PIC

    debug_log("[+] \tEnabling APIC globally\n");
    IA32_APIC_BASE base = { read_msr(MSR_CODE_IA32_APIC_BASE) };
    base.apic_global_enable = 1;
    write_msr(MSR_CODE_IA32_APIC_BASE, base.raw);

    // map lapic->proc
    for(int i = 0; ; i++) {
        madt_lapic_t* cur = madt_get_next(MADT_TYPE_LAPIC, i);
        if(cur == NULL) break;
        if(!cur->processor_enabled) continue;

        lapic_pid_map[cur->id] = cur->processor_id;
    }

    // map the ioapic addresses
    for(int i = 0; ; i++) {
        madt_ioapic_t* cur = madt_get_next(MADT_TYPE_IOAPIC, i);
        if(cur == NULL) break;

        // TODO: how much size do we actually need to map?
        if(!vmm_is_mapped(&kernel_handle, PHYSICAL_TO_DIRECT((uintptr_t)cur->mmio_base), PAGE_SIZE)) {
            vmm_map(&kernel_handle, cur->mmio_base, PHYSICAL_TO_DIRECT((uintptr_t)cur->mmio_base), PAGE_SIZE, PAGE_SUPERVISOR_READWRITE, DEFAULT_CACHE);
        }
    }
}

uint64_t apic_get_processor_id() {
    return lapic_pid_map[lapic_get_id()];
}
