#include <util/defs.h>
#include <util/except.h>
#include <arch/pmm.h>
#include "stivale.h"

uint8_t g_bootstrap_stack[SIZE_4KB] = {0};

__attribute__((used, section(".stivalehdr")))
stivale_header_t header = {
    .stack = (uint64_t) (g_bootstrap_stack + sizeof(g_bootstrap_stack)),
    .framebuffer_bpp = 32,
    .flags = BIT1
};

void init_gdt();

static const char* g_memory_map_names[] = {
    [1] = "Usable RAM",
    [2] = "Reserved",
    [3] = "ACPI reclaimable",
    [4] = "ACPI NVS",
    [5] = "Bad memory",
    [10] = "Kernel/Modules",
};

void kentry(stivale_struct_t* strct) {
    TRACE("TomatOS (build " __DATE__ " " __TIME__ ")");

    // first of all set the gdt properly
    init_gdt();

    // setup the allocator (only first 4GB)
    for (int i = 0; i < strct->memory_map_entries; i++) {
        mmap_entry_t* entry = &strct->memory_map_addr[i];
        TRACE("%016llx - %016llx: %s", entry->base, entry->base + entry->length, g_memory_map_names[entry->type]);
        if (entry->type == 1 && entry->base + entry->length < BASE_4GB) {
            pmm_submit_range(PHYSICAL_TO_DIRECT(entry->base), entry->length / PAGE_SIZE);
        }
    }

    while(1);
}
