#include <graphics/term.h>
#include <drivers/pic.h>
#include "irq.h"
#include "idt.h"
#include "interrupts.h"

#define IRQ_BASE 32

interrupt_handler_f irq_handlers[16] = {0};

void irq_init() {
    term_write("[irq_init] Remapping IRQs\n");
    pic_remap(IRQ_BASE, IRQ_BASE + 8);

    term_write("[irq_init] Settings interrupts stubs\n");
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

static const char* IRQ_NAMES[] = {
        "Programmable Interval Timer (PIT)",
        "Keyboard",
        "Cascade",
        "COM2",
        "COM1",
        "LPT2",
        "FLOPPY",
        "LPT1",
        "CMOS",
        "PERIPHERALS_1",
        "PERIPHERALS_2",
        "PERIPHERALS_3",
        "PS2 Mouse",
        "Co-processor",
        "Primary ATA",
        "Secondary ATA",
};

void irq_common(registers_t registers) {
    if(irq_handlers[registers.int_num - IRQ_BASE]) {
        irq_handlers[registers.int_num - IRQ_BASE](&registers);
    }else {
        if(registers.int_num - IRQ_BASE < sizeof(IRQ_NAMES) / sizeof(char*)) {
            term_print("[irq_common] Unhandled interrupt (%s)\n", IRQ_NAMES[registers.int_num - IRQ_BASE]);
        }else {
            term_print("[irq_common] Unhandled interrupt (irq=%d, int=%d)\n", (int) registers.int_num - IRQ_BASE, (int) registers.int_num);
        }
    }
}
