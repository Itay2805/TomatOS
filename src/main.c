#include <event/timers/timer.h>
#include <proc/process.h>
#include <intr/apic/lapic.h>
#include <acpi/acpi.h>
#include <intr/intr.h>
#include <intr/idt.h>
#include <smp/pcpu.h>

#include <util/serial.h>
#include <util/trace.h>

#include <mm/pmm.h>
#include <mm/vmm.h>
#include <mm/gdt.h>
#include <mm/mm.h>

#include <tboot.h>
#include <proc/sched.h>
#include <mm/stack_allocator.h>

static void main_thread() {
    err_t err = NO_ERROR;
    TRACE("In main thread!");

    // TESTTTTT
    event_t timer;
    CHECK_AND_RETHROW(create_event(TPL_APPLICATION, NULL, NULL, &timer));
    CHECK_AND_RETHROW(set_timer(timer, TIMER_RELATIVE, MS_TO_100NS(5000)));

    TRACE("Waiting");
    CHECK_AND_RETHROW(wait_for_event(1, &timer, NULL));
    TRACE("Done!");

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
    // TODO: idt init with the proper ists

    // convert the struct
    info = PHYSICAL_TO_DIRECT(info);

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
