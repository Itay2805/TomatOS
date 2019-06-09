#include "idt.h"
#include "interrupts.h"
#include "isr.h"
#include "irq.h"

/**
 * The idt entries
 */
static idt_entry_t idt_entries[256] = {0};

/**
 * The idt itself
 */
static idt_t idt = {
        .size = sizeof(idt_entries),
        .entries = idt_entries
};

void idt_init() {
    isr_init();
    irq_init();

    log_debug("Loading idt (entry size=%d)", (int) sizeof(idt_entry_t));
    _lidt(idt);
}

void idt_set_entry(int num, idt_handler_f handler, int type, int ist) {
    idt_entries[num].present = 1;
    idt_entries[num].handler_low = (uintptr_t)handler & 0xFFFF;
    idt_entries[num].handler_high = ((uintptr_t)handler >> 16) & 0xFFFFFFFFFFFF;
    idt_entries[num].gate_type = (uint64_t) type;
    idt_entries[num].selector = 8;
    idt_entries[num].ring = 3;
    idt_entries[num].ist = (uint64_t) ist;
}