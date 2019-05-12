#include <graphics/term.h>
#include <memory/vmm.h>
#include <drivers/pic.h>
#include <common/logging.h>
#include <common/string.h>
#include <process/process.h>
#include <process/scheduler.h>
#include "irq.h"
#include "idt.h"
#include "interrupts.h"

#define IRQ_BASE 32

interrupt_handler_f irq_handlers[16] = {0};

void irq_init() {
    LOG_INFO("Remapping IRQs");
    pic_remap(IRQ_BASE, IRQ_BASE + 8);

    LOG_INFO("Settings interrupts stubs");
    idt_set_entry(IRQ_BASE + IRQ_PIT, irq_pit, IDT_INTERRUPT_GATE);
    idt_set_entry(IRQ_BASE + IRQ_KEYBOARD, irq_keyboard, IDT_INTERRUPT_GATE);
    idt_set_entry(IRQ_BASE + IRQ_CASCADE, irq_cascade, IDT_INTERRUPT_GATE);
    idt_set_entry(IRQ_BASE + IRQ_COM2, irq_com2, IDT_INTERRUPT_GATE);
    idt_set_entry(IRQ_BASE + IRQ_COM1, irq_com1, IDT_INTERRUPT_GATE);
    idt_set_entry(IRQ_BASE + IRQ_LPT2, irq_lpt2, IDT_INTERRUPT_GATE);
    idt_set_entry(IRQ_BASE + IRQ_FLOPPY, irq_floppy, IDT_INTERRUPT_GATE);
    idt_set_entry(IRQ_BASE + IRQ_LPT1, irq_lpt1, IDT_INTERRUPT_GATE);
    idt_set_entry(IRQ_BASE + IRQ_CMOS, irq_cmos, IDT_INTERRUPT_GATE);
    idt_set_entry(IRQ_BASE + IRQ_PERIPHERALS_1, irq_peripherals_1, IDT_INTERRUPT_GATE);
    idt_set_entry(IRQ_BASE + IRQ_PERIPHERALS_2, irq_peripherals_2, IDT_INTERRUPT_GATE);
    idt_set_entry(IRQ_BASE + IRQ_PERIPHERALS_3, irq_peripherals_3, IDT_INTERRUPT_GATE);
    idt_set_entry(IRQ_BASE + IRQ_PS2_MOUSE, irq_ps2_mouse, IDT_INTERRUPT_GATE);
    idt_set_entry(IRQ_BASE + IRQ_COPROCESSOR, irq_coprocessor, IDT_INTERRUPT_GATE);
    idt_set_entry(IRQ_BASE + IRQ_PRIMARY_ATA, irq_primary_ata, IDT_INTERRUPT_GATE);
    idt_set_entry(IRQ_BASE + IRQ_SECONDARY_ATA, irq_secondary_ata, IDT_INTERRUPT_GATE);
}

void irq_common(registers_t registers) {
    error_t err = NO_ERROR;
    int int_num = (int) (registers.int_num - IRQ_BASE);
    CHECK(int_num >= 0 && int_num < 16);

    if(irq_handlers[int_num]) {
        CHECK_AND_RETHROW(irq_handlers[registers.int_num - IRQ_BASE](&registers));
    }

cleanup:
    pic_send_eoi((uint8_t)registers.int_num);
    if(IS_ERROR(err)) {
        KERNEL_PANIC(err);
    }
}
