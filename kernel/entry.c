#include <util/trace.h>
#include <util/except.h>
#include <mem/pmm.h>
#include <mem/vmm.h>
#include <task/sched.h>

#include "arch/amd64/gdt.h"
#include "arch/amd64/idt.h"

#include "arch/stivale2.h"

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
static atomic_bool g_ready = false;

static void smp_kentry(stivale2_smp_info_t* smpinfo) {
    smpinfo = PHYS_TO_DIRECT(smpinfo);

    // init basics
    init_gdt();
    init_idt();

    set_cpu_id(smpinfo->extra_argument);
    TRACE("\tGot CPU #", get_cpu_id());
    g_cpu_start_count++;

    // spin the cpu until the pmm and vmm are initialized
    // properly so we can switch paging
    while (!g_ready) {
        cpu_pause();
    }

    TRACE("\tCPU #", get_cpu_id(), " Ready");

    // init paging
    set_address_space();

    // we are ready
    g_cpu_start_count++;

    // call the handling loop, this will enter sleep
    // but once we send an ipi to wake it up it will
    // start running as well
    start_schedule_loop();
}

void kentry(stivale2_struct_t* info) {
    err_t err = NO_ERROR;
    g_stivale2_struct = info;

    //
    // quick way to see we are up and running
    //
    TRACE("Starting TomatOS [bootloader: ", info->bootloader_brand, "-", info->bootloader_version, "]");

    //
    // start with initializing basic shit
    //
    init_idt();
    init_gdt();

    //
    // initialize the pmm
    //
    CHECK_AND_RETHROW(init_pmm());

    //
    // take ownership over all the cores first
    //
    stivale2_struct_tag_smp_t* smp = get_stivale2_tag(STIVALE2_STRUCT_TAG_SMP_IDENT);
    size_t cpu_count = smp->cpu_count;
    if (smp != NULL) {
        TRACE("Take ownership over CPUs");
        for (int i = 0; i < smp->cpu_count; i++) {
            stivale2_smp_info_t* smpinfo = &smp->smp_info[i];
            if (smpinfo->lapic_id == smp->bsp_lapic_id) {
                TRACE("\tCPU #", i, " is BSP");
                set_cpu_id(i);
                g_cpu_start_count++;
            } else {
                // TODO: stack allocator
                uint8_t* stack = early_page_alloc() + PAGE_SIZE;
                smpinfo->target_stack = (uint64_t)stack;
                smpinfo->extra_argument = i;

                // must have barrier to make sure this is written
                // after the rest is written
                memory_barrier();
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

    //
    // initialize vmm
    //
    CHECK_AND_RETHROW(init_vmm());

    // we can't access these anymore at this point
    smp = NULL;
    g_stivale2_struct = NULL;

    //
    // we are going to tell all the cpus that we are ready
    // to continue operation on them
    //
    TRACE("Continuing all CPUs");
    g_cpu_start_count = 1;
    store_fence();
    g_ready = true;
    while (g_cpu_start_count != cpu_count) {
        cpu_pause();
    }

    TRACE("Done kernel early init");

cleanup:
    ASSERT(!IS_ERROR(err), "Failed early kernel initialization")
    while(1);
}
