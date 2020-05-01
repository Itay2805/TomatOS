#include <compo/fs/initrd/initrd.h>

#include <event/timers/timer.h>

#include <proc/process.h>
#include <proc/sched.h>

#include <acpi/acpi.h>

#include <intr/apic/lapic.h>
#include <intr/intr.h>
#include <intr/idt.h>
#include <smp/pcpu.h>

#include <util/serial.h>
#include <util/trace.h>

#include <mm/stack_allocator.h>
#include <mm/pmm.h>
#include <mm/vmm.h>
#include <mm/gdt.h>
#include <mm/mm.h>

#include <compo/component.h>
#include <compo/display/framebuffer/framebuffer.h>
#include <compo/display/display.h>
#include <compo/fs/vfs.h>
#include <compo/fs/fs.h>

#include <arch/simd.h>

#include <stivale.h>

/**
 * A temporary stack used until the system can boot
 */
static char temp_stack[4096] = {0};

__attribute__((section(".stivalehdr"), used)) stivale_header_t header = {
    .stack = (uintptr_t)&temp_stack + sizeof(temp_stack),
    .flags = 1,
};

static stivale_struct_t* g_stivale_struct = NULL;

/**
 * The main thread will initialize and prepare everything
 */
static void main_thread() {
    err_t err = NO_ERROR;
    TRACE("In main thread!");

    // initialize the components
    init_components();

    // only mount initrd if has any
    if (g_stivale_struct->module_count == 1) {
        TRACE("Mounting initrd");
        create_initrd_fs((stivale_module_t*)g_stivale_struct->modules);
    }

    // setup the screen device
    init_framebuffer_screen(g_stivale_struct);

    file_t handle = NULL;
    CHECK_AND_RETHROW(vfs_open("/test.elf", &handle));
    process_t* process = NULL;
    CHECK_AND_RETHROW(spawn_process(handle, &process));
    CHECK_AND_RETHROW(file_close(handle));

cleanup:
    while(1);
}

void kernel_main(stivale_struct_t* info) {
    err_t err = NO_ERROR;
    serial_init();
    TRACE("Hello from kernel, info = %p", info);

    // super early init
    init_gdt_for_bsp();
    idt_init();
    interrupts_init();
    pcpu_init_for_bsp();
    set_tpl(TPL_HIGH_LEVEL);

    // memory init
    pmm_init(info);
    vmm_init(info);
    pmm_post_vmm();
    mm_init();
    init_tss_for_cpu();
    idt_post_tss_init();

    // convert the struct
    info = PHYSICAL_TO_DIRECT(info);
    info->cmdline = PHYSICAL_TO_DIRECT(info->cmdline);
    info->memory_map_addr = PHYSICAL_TO_DIRECT(info->memory_map_addr);
    if (info->module_count > 0) {
        info->modules = PHYSICAL_TO_DIRECT(info->modules);
        stivale_module_t* module = (stivale_module_t*)info->modules;
        for (int i = 0; i < info->module_count; i++) {
            module->begin = PHYSICAL_TO_DIRECT(module->begin);
            module->end = PHYSICAL_TO_DIRECT(module->end);
            module->next = PHYSICAL_TO_DIRECT(module->next);
            module = (stivale_module_t*)module->next;
        }
    }
    g_stivale_struct = info;

    // do BSP init
    init_lapic_for_cpu();
    init_syscalls_for_cpu();
    init_simd_for_bsp();

    // early kernel init
    init_kernel_process();
    acpi_tables_init(info->rsdp);
    events_init();
    init_timer();

    // TODO: smp

    // initialize scheduler and start a main thread for the kernel
    CHECK_AND_RETHROW(init_sched());
    CHECK_AND_RETHROW(spawn_thread(&kernel_process, (uintptr_t)main_thread, allocate_stack(), NULL));

    // kickstart everything by restoring the tpl
    // this will cause the scheduler to yield and
    // everything should just start nicely
    TRACE("Starting threading");
    restore_tpl(TPL_APPLICATION);
    while(1) asm volatile ("hlt" ::: "memory");

cleanup:
    ASSERT(!"Early kernel init failed, halting system");
}
