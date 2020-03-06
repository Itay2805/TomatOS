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
#include <compo/fs/vfs.h>
#include <compo/fs/fs.h>

#include <tboot.h>

static tboot_info_t* g_info;

/**
 * The main thread will initialize and prepare everything
 */
static void main_thread() {
    err_t err = NO_ERROR;
    TRACE("In main thread!");

    // start initializations
    TRACE("Mounting initrd");
    ASSERT(g_info->modules.count == 1);
    create_initrd_fs(&g_info->modules.entries[0]);

    // spawn a process from an elf file
    file_t file = NULL;
    process_t* proc = NULL;
    CHECK_AND_RETHROW(vfs_open("/test.elf", &file));
    CHECK_AND_RETHROW(spawn_process(file, &proc));
    TRACE("Spawned test");

cleanup:
    while(1);
}

void kernel_main(uint32_t magic, tboot_info_t* info) {
    err_t err = NO_ERROR;
    serial_init();
    TRACE("Hello from kernel, magic = %x", magic);

    // super early init
    idt_init();
    interrupts_init();
    pcpu_init_for_bsp();

    // memory initialization
    pmm_init(info);
    vmm_init(info);
    pmm_post_vmm();
    mm_init();
    init_tss_for_cpu();
    idt_post_tss_init();
    // TODO: idt init with the proper ists

    // convert the struct
    info = PHYSICAL_TO_DIRECT(info);
    info->modules.entries = PHYSICAL_TO_DIRECT(info->modules.entries);
    g_info = info;

    // BSP init
    lapic_init();

    // early kernel init
    init_kernel_process();
    acpi_tables_init(info);
    events_init();
    init_timer();
    init_sched();

    // TODO: smp

    // setup main thread
    // will start at high level, should make everything play nicely when doing
    // the yield to start with the tasking. also make sure interrupts are off
    // by default, they will be enabled by the thread when going back to low tpl
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
