#include <util/defs.h>
#include <util/except.h>
#include <mem/pmm.h>
#include <stdbool.h>
#include <mem/mm.h>
#include <acpi/acpi.h>
#include <arch/timing.h>
#include <lai/helpers/sci.h>
#include <util/stb_ds.h>
#include <driver/pci/pci.h>
#include <arch/cpu.h>
#include <proc/scheduler.h>
#include <stdnoreturn.h>
#include <proc/event.h>
#include <driver/driver.h>
#include "stivale.h"
#include "apic.h"

__attribute__((section(".stivale_stack")))
uint8_t g_bootstrap_stack[SIZE_16KB] = {0};

__attribute__((used, section(".stivalehdr")))
stivale_header_t header = {
    .stack = (uint64_t) (g_bootstrap_stack + sizeof(g_bootstrap_stack)),
    .framebuffer_bpp = 32,
    .flags = BIT1
};

void init_gdt();
void init_tss();
err_t init_vmm(stivale_struct_t* strct);
void init_cpu_local_for_bsp();
err_t init_cpu_local();
void init_idt();
err_t vmm_figure_features();

static const char* g_memory_map_names[] = {
    [1] = "Usable RAM",
    [2] = "Reserved",
    [3] = "ACPI reclaimable",
    [4] = "ACPI NVS",
    [5] = "Bad memory",
    [10] = "Kernel/Modules",
};

static const char* g_size_names[] = { "B", "kB", "MB", "GB" };

event_t event = NULL;

static void main_thread(void* arg) {
    err_t err = NO_ERROR;

    TRACE("In kernel thread!");
    CHECK_AND_RETHROW(dispatch_drivers());

cleanup:
    ASSERT_TRACE(!IS_ERROR(err), "Got an error in main thread");
}

noreturn void kentry(stivale_struct_t* strct) {
    err_t err = NO_ERROR;
    size_t available_size = 0;
    size_t size = 0;
    size_t div = 0;

    // first of all set the gdt properly
    init_gdt();
    init_cpu_local_for_bsp();

    TRACE("TomatOS (build " __DATE__ " " __TIME__ ")");
#ifdef __TOMATOS_DEBUG__
    TRACE("\t* Debug");
#endif

    // setup the allocator (only first 4GB)
    TRACE("Boostraping memory");
    for (int i = 0; i < strct->memory_map_entries; i++) {
        mmap_entry_t* entry = &strct->memory_map_addr[i];
        TRACE("\t%016llx - %016llx: %s", entry->base, entry->base + entry->length, g_memory_map_names[entry->type]);
        if (entry->type == 1 && entry->base + entry->length < BASE_4GB) {
            pmm_submit_range(PHYSICAL_TO_DIRECT(entry->base), entry->length / PAGE_SIZE);
            available_size += entry->length;
        }
    }

    size = available_size;
    while (size >= 1024 && div < ARRAY_LENGTH(g_size_names)) {
        div++;
        size /= 1024;
    }
    TRACE("Bootstrap memory size: %d %s", size, g_size_names[div]);

    // setup the kernel paging
    CHECK_AND_RETHROW(init_vmm(strct));

    // convert relevant pointers
    strct = PHYSICAL_TO_DIRECT(strct);
    strct->rsdp = (uint64_t)PHYSICAL_TO_DIRECT(strct->rsdp);
    strct->cmdline = PHYSICAL_TO_DIRECT(strct->cmdline);
    strct->memory_map_addr = PHYSICAL_TO_DIRECT(strct->memory_map_addr);

    // finish setup the pmm entries
    for (int i = 0; i < strct->memory_map_entries; i++) {
        mmap_entry_t* entry = &strct->memory_map_addr[i];
        if (entry->type == 1 && entry->base >= BASE_4GB) {
            pmm_submit_range(PHYSICAL_TO_DIRECT(entry->base), entry->length / PAGE_SIZE);
            available_size += entry->length;
        }
    }

    size = available_size;
    div = 0;
    while (size >= 1024 && div < ARRAY_LENGTH(g_size_names)) {
        div++;
        size /= 1024;
    }
    TRACE("Available memory size: %d %s", size, g_size_names[div]);

    // initialize the kernel allocator and
    // cpu locals, in preparation for threading
    CHECK_AND_RETHROW(mm_init());
    init_tss();
    init_idt();

    // the kernel is the current process
    g_current_process = &g_kernel;

    // tell acpi which ranges it can access
    for (int i = 0; i < strct->memory_map_entries; i++) {
        mmap_entry_t* entry = &strct->memory_map_addr[i];
        memmap_entry_t reg = {
            .base = entry->base,
            .end = entry->base + entry->length,
            .acpi_access = entry->type == 2 || entry->type == 3 || entry->type == 4,
        };
        arrpush(g_memory_map, reg);
    }

    // initialize acpi and apic
    init_acpi_tables(strct->rsdp);
    CHECK_AND_RETHROW(init_apic());

    // initialize scheduler
    // this is done here so we can start
    // queueing threads
    int cpu_count = 0;
    FOR_EACH_IN_MADT() {
        if (entry->type != MADT_LAPIC) continue;
        if (!entry->lapic.enabled && !entry->lapic.online_capable) continue;
        cpu_count++;
    }
    init_scheduler(cpu_count);

    // initialize other stuff
    CHECK_AND_RETHROW(init_pci());
    CHECK_AND_RETHROW(init_acpi());

    // initialize smp
    init_lapic();
    g_cpu_id = get_lapic_id();
    CHECK_AND_RETHROW(startup_all_cores());

    // initialize other services we need
    CHECK_AND_RETHROW(init_ripper());

    // start drivers dispatching thread
    thread_t* thread;
    CHECK_AND_RETHROW(create_thread(&thread, main_thread, NULL, NULL));
    schedule_thread(thread);
    CHECK_AND_RETHROW(close_thread(thread));

    // allocate a stack for the idle thread and
    // start the scheduler
    g_idle_stack = (uintptr_t)alloc_stack();
    startup_scheduler();

cleanup:
    ASSERT_TRACE(false, "Error during kernel initialization");
}

noreturn void per_cpu_entry(uintptr_t stack) {
    err_t err = NO_ERROR;

    // initialize locals properly
    CHECK_AND_RETHROW(init_cpu_local());
    g_cpu_id = get_lapic_id();
    g_current_process = &g_kernel;
    g_idle_stack = stack;

    CHECK_AND_RETHROW(vmm_figure_features());
    init_tss();

    // setup the lapic
    init_lapic();

    // startup scheduler
    startup_scheduler();

cleanup:
    ASSERT_TRACE(false, "Error during kernel initialization");
}
