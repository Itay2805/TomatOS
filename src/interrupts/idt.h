#ifndef KRETLIM_KERNEL_IDT_H
#define KRETLIM_KERNEL_IDT_H

#include <stdint.h>

#define IDT_TYPE_TASK           0x85
#define IDT_TYPE_INTERRUPT_16   0x86
#define IDT_TYPE_TRAP_16        0x87
#define IDT_TYPE_INTERRUPT_32   0x8E
#define IDT_TYPE_TRAP_32        0x8F

typedef struct idt_entry {
    struct {
        uint64_t handler_low : 16;
        uint64_t selector : 16;
        uint64_t ist : 3;
        uint64_t zero1 : 5;
        uint64_t gate_type : 8;
        uint64_t handler_high : 48;
        uint64_t zero3 : 32;
    } __attribute__((packed));
    uint64_t raw;
} idt_entry_t;

typedef struct idt {
    uint16_t limit;
    idt_entry_t* base;
} __attribute__((packed)) idt_t;

extern idt_entry_t idt_entries[0xFF];
extern idt_t idt;

/**
 * This will set all the stub handlers
 */
void idt_init();

#endif //KRETLIM_KERNEL_IDT_H
