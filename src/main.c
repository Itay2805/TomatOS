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
#include <util/stall.h>
#include <processes/process.h>
#include <processes/thread.h>
#include <processes/scheduler.h>
#include <lai/helpers/sci.h>

static thread_t init_thread = {0};

static void kernel_init_thread() {
    debug_log("[+] In init thread!\n");

    debug_log("[*] Initializing ACPI\n");
    lai_create_namespace();
    ASSERT(!lai_enable_acpi(1));

    while(true);
}

void kernel_main(uint32_t magic, tboot_info_t* info) {
    debug_log("[+] Entered kernel!\n");

    // we can init these right away
    stall_init(info);
    interrupts_init();
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

    // prepare the storage
    percpu_storage_init();

    // finish the initialization of the bsp
    debug_log("[*] Finishing BSP init\n");
    lapic_init();
    tss_init();

    // init kernel process and such
    kernel_process_init();
    thread_init(&init_thread, &kernel_process);
    init_thread.cpu_state.rip = (uint64_t)kernel_init_thread;
    scheduler_queue_thread(&init_thread);

    // do smp!
    smp_startup(info);

    // and now finish by starting our scheduler and
    // kicks starting it on all cores
    debug_log("[+] Scheduler startup!\n");
    per_cpu_scheduler_init();

    // enable interrupts and send the ipi
    enable_interrupts();
    lapic_send_ipi_all_including_self(IPI_SCHEDULER_STARTUP);

    ASSERT(false);
}
