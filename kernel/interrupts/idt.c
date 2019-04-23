#include <drivers/pic.h>
#include <graphics/term.h>
#include "idt.h"
#include "irq.h"
#include "isr.h"
#include "interrupts.h"

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
    term_print("[idt_init] Loading idt (entry size=%d)\n", (int) sizeof(idt_entry_t));
    lidt(&idt);
}

void idt_set_entry(int num, idt_handler_f handler, int type) {
    idt_entries[num].present = 1;
    idt_entries[num].handler_low = (uintptr_t)handler & 0xFFFF;
    idt_entries[num].handler_high = ((uintptr_t)handler >> 16) & 0xFFFFFFFFFFFF;
    idt_entries[num].gate_type = (uint64_t) type;
    idt_entries[num].selector = 8;
}