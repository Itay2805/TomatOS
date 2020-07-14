#include <util/defs.h>
#include <util/except.h>
#include <mem/pmm.h>
#include <stdbool.h>
#include <mem/mm.h>
#include "stivale.h"
#include "intrin.h"

uint8_t g_bootstrap_stack[SIZE_4KB] = {0};

__attribute__((used, section(".stivalehdr")))
stivale_header_t header = {
    .stack = (uint64_t) (g_bootstrap_stack + sizeof(g_bootstrap_stack)),
    .framebuffer_bpp = 32,
    .flags = BIT1
};

void init_gdt();
err_t init_vmm(stivale_struct_t* strct);
void init_cpu_local_for_bsp();
err_t init_idt();

static const char* g_memory_map_names[] = {
    [1] = "Usable RAM",
    [2] = "Reserved",
    [3] = "ACPI reclaimable",
    [4] = "ACPI NVS",
    [5] = "Bad memory",
    [10] = "Kernel/Modules",
};

static const char* g_size_names[] = { "B", "kB", "MB", "GB" };

void kentry(stivale_struct_t* strct) {
    err_t err = NO_ERROR;
    size_t available_size = 0;
    size_t size = 0;
    size_t div = 0;

    TRACE("TomatOS (build " __DATE__ " " __TIME__ ")");

    // first of all set the gdt properly
    init_gdt();

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
    init_cpu_local_for_bsp();
    CHECK_AND_RETHROW(mm_init());
    CHECK_AND_RETHROW(init_idt());

cleanup:
    ASSERT_TRACE(!IS_ERROR(err), "Error during kernel initialization");
    TRACE("kernel entry end");
    while(true) __hlt();
}
