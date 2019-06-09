#include <drivers/apic/lapic.h>
#include <buf.h>
#include <map.h>
#include "irq.h"
#include "idt.h"

static map_t irq_handler_map;

extern void irq_0();
extern void irq_1();
extern void irq_2();
extern void irq_3();
extern void irq_4();
extern void irq_5();
extern void irq_6();
extern void irq_7();
extern void irq_8();
extern void irq_9();
extern void irq_10();
extern void irq_11();
extern void irq_12();
extern void irq_13();
extern void irq_14();
extern void irq_15();
extern void irq_16();
extern void irq_17();
extern void irq_18();
extern void irq_19();
extern void irq_20();
extern void irq_21();
extern void irq_22();
extern void irq_23();

void irq_init() {
    log_debug("Setting interrupt stubs");
    idt_set_entry(INTERRUPT_IRQ_BASE + 0, irq_0, IDT_INTERRUPT_GATE, 0);
    idt_set_entry(INTERRUPT_IRQ_BASE + 1, irq_1, IDT_INTERRUPT_GATE, 0);
    idt_set_entry(INTERRUPT_IRQ_BASE + 2, irq_2, IDT_INTERRUPT_GATE, 0);
    idt_set_entry(INTERRUPT_IRQ_BASE + 3, irq_3, IDT_INTERRUPT_GATE, 0);
    idt_set_entry(INTERRUPT_IRQ_BASE + 4, irq_4, IDT_INTERRUPT_GATE, 0);
    idt_set_entry(INTERRUPT_IRQ_BASE + 5, irq_5, IDT_INTERRUPT_GATE, 0);
    idt_set_entry(INTERRUPT_IRQ_BASE + 6, irq_6, IDT_INTERRUPT_GATE, 0);
    idt_set_entry(INTERRUPT_IRQ_BASE + 7, irq_7, IDT_INTERRUPT_GATE, 0);
    idt_set_entry(INTERRUPT_IRQ_BASE + 8, irq_8, IDT_INTERRUPT_GATE, 0);
    idt_set_entry(INTERRUPT_IRQ_BASE + 9, irq_9, IDT_INTERRUPT_GATE, 0);
    idt_set_entry(INTERRUPT_IRQ_BASE + 10, irq_10, IDT_INTERRUPT_GATE, 0);
    idt_set_entry(INTERRUPT_IRQ_BASE + 11, irq_11, IDT_INTERRUPT_GATE, 0);
    idt_set_entry(INTERRUPT_IRQ_BASE + 12, irq_12, IDT_INTERRUPT_GATE, 0);
    idt_set_entry(INTERRUPT_IRQ_BASE + 13, irq_13, IDT_INTERRUPT_GATE, 0);
    idt_set_entry(INTERRUPT_IRQ_BASE + 14, irq_14, IDT_INTERRUPT_GATE, 0);
    idt_set_entry(INTERRUPT_IRQ_BASE + 15, irq_15, IDT_INTERRUPT_GATE, 0);
    idt_set_entry(INTERRUPT_IRQ_BASE + 16, irq_16, IDT_INTERRUPT_GATE, 0);
    idt_set_entry(INTERRUPT_IRQ_BASE + 17, irq_17, IDT_INTERRUPT_GATE, 0);
    idt_set_entry(INTERRUPT_IRQ_BASE + 18, irq_18, IDT_INTERRUPT_GATE, 0);
    idt_set_entry(INTERRUPT_IRQ_BASE + 19, irq_19, IDT_INTERRUPT_GATE, 0);
    idt_set_entry(INTERRUPT_IRQ_BASE + 20, irq_20, IDT_INTERRUPT_GATE, 0);
    idt_set_entry(INTERRUPT_IRQ_BASE + 21, irq_21, IDT_INTERRUPT_GATE, 0);
    idt_set_entry(INTERRUPT_IRQ_BASE + 22, irq_22, IDT_INTERRUPT_GATE, 0);
    idt_set_entry(INTERRUPT_IRQ_BASE + 23, irq_23, IDT_INTERRUPT_GATE, 0);
}

void irq_common(registers_t regs) {
    error_t err = NO_ERROR;

    // save the address space
    regs.cr3 = vmm_get();
    if(regs.cr3 != kernel_address_space) {
        vmm_set(kernel_address_space);
    }

    interrupt_handler_f* handlers = map_get_from_uint64(&irq_handler_map, regs.int_num);
    for(interrupt_handler_f* handler = handlers; handler < buf_end(handlers); handler++) {
        CHECK_AND_RETHROW((*handler)(&regs));
    }

    // send end of interrupt, even if failed
    CHECK_AND_RETHROW(lapic_send_eoi());

cleanup:
    if(err != NO_ERROR) {
        log_critical("Got error in IRQ handler, halting kernel :(");
        _cli();
        while(true) {
            _hlt();
        }
    }

    // restore the address space
    if(regs.cr3 != kernel_address_space) {
        vmm_set(regs.cr3);
    }
}

error_t irq_set_handler(uint32_t irq, interrupt_handler_f handler) {
    interrupt_handler_f* handlers = map_get_from_uint64(&irq_handler_map, irq);
    buf_push(handlers, handler);
    map_put_from_uint64(&irq_handler_map, irq, handlers);
    return NO_ERROR;
}

