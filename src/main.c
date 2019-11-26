#include <processes/scheduler.h>
#include <processes/process.h>
#include <processes/thread.h>

#include <interrupts/apic/lapic.h>
#include <interrupts/apic/apic.h>

#include <smp/percpu_storage.h>
#include <smp/smp.h>

#include <drivers/storage/ahci/ahci.h>
#include <drivers/bus/pci/pci.h>

#include <memory/pmm.h>
#include <memory/vmm.h>

#include <util/debug.h>
#include <util/stall.h>
#include <acpi/acpi.h>

#include <stdbool.h>
#include <stdint.h>
#include <tboot.h>
#include <drivers/storage/storage_object.h>
#include <drivers/partition/partition.h>

__attribute__((unused))
static thread_t* init_thread = NULL;

__attribute__((unused))
static void kernel_init_thread() {
    debug_log("[+] In init thread!\n");
    acpi_init();

    // start by doing bus initialization (other drivers might use it)
    debug_log("[+] Loading bus drivers\n");
    pci_init();

    // do storage device initialization
    debug_log("[+] Loading storage drivers\n");
    ahci_scan();

    // mount all of the storage devices
    debug_log("[+] Mounting partitions\n");
    for(list_entry_t* link = storage_objects.next; link != &storage_objects; link = link->next) {
        storage_device_t* storage = CR(link, storage_device_t, device.link);
        debug_log("[*] attempting to mount `%s`\n", storage->device.name);
        storage_mount(storage);
    }

    debug_log("[+] Mounting filesystems\n");
    for(list_entry_t* slink = storage_objects.next; slink != &storage_objects; slink = slink->next) {
        storage_device_t* storage = CR(slink, storage_device_t, device.link);

        for (list_entry_t* plink = storage->partitions.next; plink != &storage->partitions; plink = plink->next) {
            partition_t* partition = CR(plink, partition_t, device.link);

            debug_log("[*] attempting to mount `%s`\n", partition->device.name);
            partition_mount(partition);
        }
    }

    debug_log("[+] Driver initialization finished!\n");

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
    kernel_process = new_process();
    kernel_process->vmm_handle = kernel_handle;
//    init_thread = new_thread(kernel_process, kernel_init_thread);
//    scheduler_queue_thread(init_thread);

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
