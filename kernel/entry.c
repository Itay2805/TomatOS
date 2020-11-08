
#include <util/trace.h>
#include <util/except.h>
#include <mem/pmm.h>
#include <mem/vmm.h>
#include "arch/stivale2.h"
#include "arch/gdt.h"
#include "arch/idt.h"

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

void kentry(stivale2_struct_t* info) {
    err_t err = NO_ERROR;
    g_stivale2_struct = info;

    // start with initializing basic shit
    init_idt();
    init_gdt();

    TRACE("Starting TomatOS [bootloader: ", info->bootloader_brand, "-", info->bootloader_version, "]");

    // initialize pmm and gd
    stivale2_struct_tag_memmap_t* memmap = get_stivale2_tag(STIVALE2_STRUCT_TAG_MEMMAP_IDENT);
    init_pmm(memmap);
    CHECK_AND_RETHROW(init_vmm(memmap));

    TRACE("Done early init!");

cleanup:
    ASSERT(!IS_ERROR(err), "Failed early kernel initialization")
    while(1);
}
