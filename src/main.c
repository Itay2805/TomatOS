#include <stdint.h>
#include <stdbool.h>

#include <memory/pmm.h>
#include <util/debug.h>
#include <tboot.h>
#include <memory/vmm.h>
#include <smp/smp.h>
#include <acpi/acpi.h>
#include <interrupts/apic/apic.h>
#include <interrupts/apic/lapic.h>
#include <smp/percpu_storage.h>

void kernel_main(uint32_t magic, tboot_info_t* info) {
    debug_log("[+] Entered kernel!\n");

    // we can init the idt right away
    idt_init();

    // do memory initialization
    pmm_init(info);
    vmm_init(info);
    pmm_post_vmm();

    // convert the info pointer
    info = PHYSICAL_TO_DIRECT(info);

    // init apic related stuff
    acpi_tables_init(info);
    apic_init();
    lapic_init(info->tsc_freq);

    // init mp
    percpu_storage_init();
    smp_startup();

    // TODO: do scheduler startup

    ASSERT(false);
}
