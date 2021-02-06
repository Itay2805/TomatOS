#include <util/trace.h>
#include <util/except.h>
#include <util/string.h>
#include <mem/pmm.h>
#include <mem/vmm.h>
#include <task/sched.h>

#include "arch/gdt.h"
#include "arch/idt.h"

#include "arch/stivale2.h"
#include "main.h"

static char g_bootstrap_stack[SIZE_4KB];

stivale2_header_tag_framebuffer_t g_stibale2_framebuffer = {
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
        .next = 0,
    },
    .framebuffer_width = 0,
    .framebuffer_height = 0,
    .framebuffer_bpp = 32,
};

stivale2_header_tag_smp_t g_stibale2_smp = {
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_SMP_ID,
        .next = (uint64_t) &g_stibale2_framebuffer.tag,
    },
};

__attribute__((used, section(".stivale2hdr")))
static stivale2_header_t header = {
    .stack = (uint64_t)(g_bootstrap_stack + SIZE_4KB),
    .tags = (uint64_t) &g_stibale2_smp.tag
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The kernel entries and setup
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * For signaling when all cores are done initializing
 */
static atomic_int g_cpu_start_count = 0;

/**
 * For signalling cores the kernel is done initializing and is ready to continue
 */
static atomic_bool g_ready = false;

static void smp_kentry(volatile stivale2_smp_info_t* smpinfo) {
    smpinfo = PHYS_TO_DIRECT(smpinfo);

    // init basics
    init_gdt();
    init_idt();

    // set the cpu locals and print
    init_cpu_locals(smpinfo->extra_argument, smpinfo->lapic_id);
    TRACE("\tGot CPU #%d (lapicid=%d) ", g_cpu_id, g_lapic_id);

    // signal we are done
    g_cpu_start_count++;

    // we want everything to be done till here
    memory_barrier();

    // spin the cpu until the pmm and vmm are initialized
    // properly so we can switch paging
    while (!g_ready) {
        cpu_pause();
    }

    TRACE("\tCPU #%d Ready", g_cpu_id, g_lapic_id);

    // init paging
    set_address_space();
    init_lapic();

    // we are ready
    g_cpu_start_count++;

    // call the handling loop, this will enter sleep
    // but once we send an ipi to wake it up it will
    // start running as well
    task_dispatcher();
}

/**
 * Size of cpu locals
 */
extern char __cpu_local_size[];

void kentry(stivale2_struct_t* info) {
    err_t err = NO_ERROR;
    g_stivale2_struct = info;

    //
    // quick way to see we are up and running
    //
    UNLOCKED_TRACE("Starting TomatOS [bootloader: %s-%s]", info->bootloader_brand, info->bootloader_version);

    //
    // start with initializing basic shit
    //
    init_idt();
    init_gdt();

    //
    // take ownership over all the cores first
    //
    stivale2_struct_tag_smp_t* smp = get_stivale2_tag(STIVALE2_STRUCT_TAG_SMP_ID);

    //
    // init the cpu local storage buffers
    //
    g_cpu_count = smp->cpu_count;
    size_t total_cpu_locals_size = g_cpu_count * ((uintptr_t)__cpu_local_size);
    uintptr_t for_cpu_locals = (uintptr_t)early_alloc(SIZE_TO_PAGES(total_cpu_locals_size));
    memset((void*)for_cpu_locals, 0, total_cpu_locals_size);
    for (int i = 0; i < g_cpu_count; i++) {
        g_cpu_locals[i] = (uintptr_t)for_cpu_locals + i * ((uintptr_t)__cpu_local_size);
    }

    //
    // Iterate the cpus and take ownership over them
    //
    if (smp != NULL) {
        UNLOCKED_TRACE("Take ownership over CPUs");
        for (int i = 0; i < smp->cpu_count; i++) {
            stivale2_smp_info_t* smpinfo = &smp->smp_info[i];
            if (smpinfo->lapic_id == smp->bsp_lapic_id) {
                UNLOCKED_TRACE("\tCPU #%d is BSP (lapic=%d)", i, smpinfo->lapic_id);
                g_cpu_start_count++;

                // set the locals for this cpu (aka bsp)
                init_cpu_locals(i, smpinfo->lapic_id);
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
        UNLOCKED_TRACE("Done SMP startup");
    } else {
        // TODO: Get the id of the current lapic using an msr
        init_cpu_locals(0, 0);
        WARN("Missing SMP info, will only have one core.");
    }

    //
    // initialize the pmm and vmm
    //
    CHECK_AND_RETHROW(init_pmm());
    CHECK_AND_RETHROW(init_vmm());
    init_lapic();

    //
    // Initialize the task dispatcher
    //
    init_task_dispatcher();

    // we can't access these anymore at this point
    smp = NULL;
    g_stivale2_struct = NULL;

    //
    // we are going to tell all the cpus that we are ready
    // to continue operation on them
    //
    TRACE("Continuing all CPUs");

    // core does not need to start
    g_cpu_start_count = 1;
    store_fence();

    // signal we are ready and wait
    g_ready = true;
    while (g_cpu_start_count != g_cpu_count) {
        cpu_pause();
    }

    TRACE("Done kernel early init");

    // queue the main task so we can continue setup in there
    queue_main_task();

    // start the scheduler loop
    task_dispatcher();

cleanup:
    ASSERT(!IS_ERROR(err), "Failed early kernel initialization")
    while(1);
}
