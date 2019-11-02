#include <libc/stddef.h>
#include <libc/string.h>
#include <memory/vmm.h>
#include <interrupts/apic/lapic.h>
#include <memory/gdt.h>
#include <acpi/tables/madt.h>
#include <util/poke.h>
#include <util/stall.h>
#include "smp.h"
#include "percpu_storage.h"

#define SMP_FLAG                (0x510 + DIRECT_MAPPING_BASE)
#define SMP_KERNEL_ENTRY        (0x520 + DIRECT_MAPPING_BASE)
#define SMP_KERNEL_PAGE_TABLE   (0x540 + DIRECT_MAPPING_BASE)
#define SMP_STACK_POINTER       (0x550 + DIRECT_MAPPING_BASE)
#define SMP_KERNEL_GDT          (0x580 + DIRECT_MAPPING_BASE)
#define SMP_KERNEL_IDT          (0x590 + DIRECT_MAPPING_BASE)

extern char smp_trampoline[];
extern uint64_t smp_trampoline_size;

/**
 * this is where the trampoline will jump to after
 * doing the initial stuff
 *
 * it will make sure to init all the rest of the services that are needed for the cpu to work nicely
 */
static void per_cpu_initialization() {
    lapic_init();
    tss_init();
}

void smp_startup(tboot_info_t* info) {
    /*************************************
     * Prepare the smp
     *************************************/
    debug_log("[*] Preparing SMP\n");

    // start by copying the trampoline shellcode
    memcpy(PHYSICAL_TO_DIRECT((void*)0x1000), smp_trampoline, smp_trampoline_size);
    vmm_map(&kernel_handle, 0, 0, PAGE_SIZE * 10, PAGE_SUPERVISOR_EXEC_READWRITE | PAGE_MAP_ZERO, DEFAULT_CACHE);

    // set the common stuff
    POKE64(SMP_KERNEL_ENTRY) = (uintptr_t)&per_cpu_initialization;
    POKE64(SMP_KERNEL_PAGE_TABLE) = kernel_handle.pml4_physical;
    POKE(gdt_t, SMP_KERNEL_GDT) = gdt;
    POKE(idt_t, SMP_KERNEL_IDT) = idt;

    /*************************************
     * Start the smp
     *************************************/
    debug_log("[*] SMP starting\n");

    // start with bsp
    per_cpu_initialization();

    // continue to wake up the rest of the cores
    for(int i = 0; ; i++) {
        madt_lapic_t* lapic = madt_get_next(MADT_TYPE_LAPIC, i);
        if(lapic == NULL) break;
        if(lapic->id == lapic_get_id() || !lapic->processor_enabled) continue;

        // setup the ap specific stuff
        POKE64(SMP_FLAG) = 0;
        POKE64(SMP_STACK_POINTER) = (uintptr_t)get_percpu_storage_of(lapic->id)->kernel_stack;

        /*************************************
         * SMP magic
         *************************************/

        // send the init to reset the cpu
        lapic_send_init(lapic->id);
        stall(10);

        // send the startup
        lapic_send_sipi(lapic->id, 0x1000);
        stall(1);

        if(POKE64(SMP_FLAG) == 0) {
            // if had problem try again and wait a hot second
            lapic_send_sipi(lapic->id, 0x1000);
            stall(1000);

            ASSERT(POKE64(SMP_FLAG) != 0);
        }
    }

    // TODO: finishing touches like change the idt to be post tss and alike
}
