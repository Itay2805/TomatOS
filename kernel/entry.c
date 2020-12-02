
#include <util/trace.h>
#include <util/except.h>
#include <mem/pmm.h>
#include <mem/vmm.h>
#include <proc/proc.h>
#include "arch/stivale2.h"
#include "arch/amd64/gdt.h"
#include "arch/amd64/idt.h"

static char g_bootstrap_stack[SIZE_4KB];

stivale2_header_tag_framebuffer_t g_stibale2_framebuffer = {
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_IDENT,
        .next = NULL,
    },
    .width = 0,
    .height = 0,
    .bpp = 32,
};

stivale2_header_tag_smp_t g_stibale2_smp = {
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_SMP_IDENT,
        .next = &g_stibale2_framebuffer.tag,
    },
};

__attribute__((used, section(".stivale2hdr")))
static stivale2_header_t header = {
    .stack = (uint64_t)(g_bootstrap_stack + SIZE_4KB),
    .tags = &g_stibale2_smp.tag
};

static atomic_int g_cpu_start_count = 0;

static void smp_kentry(stivale2_smp_info_t* smpinfo) {
    smpinfo = PHYS_TO_DIRECT(smpinfo);

    // init basics
    init_gdt();
    init_idt();

    // init paging
    set_address_space(&g_kernel.address_space);

    g_cpu_start_count++;
    TRACE("\tStarted CPU #", smpinfo->extra_argument);

    while (1) {
        cpu_pause();
    }
}

void kentry(stivale2_struct_t* info) {
    err_t err = NO_ERROR;
    g_stivale2_struct = info;

    // we need to convert the addresses in the stivale2
    // info to be virtual addresses
    stivale2_to_higher_half();

    // start with initializing basic shit
    init_idt();
    init_gdt();

    TRACE("Starting TomatOS [bootloader: ", info->bootloader_brand, "-", info->bootloader_version, "]");

    // initialize pmm and gd
    stivale2_struct_tag_memmap_t* memmap = get_stivale2_tag(STIVALE2_STRUCT_TAG_MEMMAP_IDENT);
    init_pmm(memmap);
    CHECK_AND_RETHROW(init_vmm(memmap));

    // print the mapping ranges for the lols
    dump_kernel_mappings();

    TRACE("Done early init!");

    // go into the other cpus if the bootloader supports SMP, otherwise
    // we will not do smp at all
    stivale2_struct_tag_smp_t* smp = get_stivale2_tag(STIVALE2_STRUCT_TAG_SMP_IDENT);
    if (smp != NULL) {
        TRACE("Starting CPUs");
        for (int i = 0; i < smp->cpu_count; i++) {
            stivale2_smp_info_t* smpinfo = &smp->smp_info[i];
            if (smpinfo->lapic_id == smp->bsp_lapic_id) {
                TRACE("\tCPU #", i, " is BSP");
                g_cpu_start_count++;
            } else {
                // TODO: stack allocator
                uint8_t* stack = page_alloc() + PAGE_SIZE;
                smpinfo->target_stack = (uint64_t)stack;
                smpinfo->extra_argument = i;

                // must have barrier to make sure this is written
                // after the rest is written
                memory_fence();
                smpinfo->goto_address = (uint64_t)smp_kentry;
            }
        }

        // wait for all cpus to finish setting up
        while (g_cpu_start_count != smp->cpu_count) {
            cpu_pause();
        }
        TRACE("Done SMP startup");
    } else {
        WARN("Missing SMP info");
    }

    // TODO: reclaim bootloader pages

cleanup:
    ASSERT(!IS_ERROR(err), "Failed early kernel initialization")
    while(1);
}
