#ifndef INTERRUPTS_IDT_H
#define INTERRUPTS_IDT_H

////////////////////////////////////////////////////////////////////////////
// Interrupt Descriptor Table
////////////////////////////////////////////////////////////////////////////
//
// This just handles the IDT itself, allows for adding entries and such
//
////////////////////////////////////////////////////////////////////////////

#include <common/stdint.h>

/**
 * This is an AMD64 idt entry
 */
typedef struct idt_entry {
    uint64_t handler_low : 16;
    uint64_t selector : 16;
    uint64_t ist : 3;
    uint64_t zero1 : 5;
    uint64_t gate_type : 4;
    uint64_t zero2 : 1;
    uint64_t ring : 2;
    uint64_t present : 1;
    uint64_t handler_high : 48;
    uint64_t zero3 : 32;
} __attribute__((packed)) idt_entry_t;

/**
 * The idt structure
 */
typedef struct idt {
    uint16_t size;
    idt_entry_t* entries;
} __attribute__((packed)) idt_t;

/**
 * This is a handler called for an interrupt
 */
typedef void(*idt_handler_f)();

typedef enum idt_type {
    IDT_INTERRUPT_GATE = 0xe,
    IDT_TRAP_GATE = 0xf
} idt_type_t;

/**
 * Initialize the idt
 *
 * will also initialize the interrupts and exceptions
 */
void idt_init();

/**
 * set an idt entry
 */
void idt_set_entry(int num, idt_handler_f handler, int type);

#endif
