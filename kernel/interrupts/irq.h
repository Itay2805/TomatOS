#ifndef INTERRUPTS_IRQ_H
#define INTERRUPTS_IRQ_H

////////////////////////////////////////////////////////////////////////////
// Interrupt Request
////////////////////////////////////////////////////////////////////////////
//
// This just handles IRQs and the pic
//
// It will also make sure the remap the irqs to not collide with isrs
//
////////////////////////////////////////////////////////////////////////////

#include <common/stdint.h>
#include "interrupts.h"

#define IRQ_PIT             (0)
#define IRQ_KEYBOARD        (1)
#define IRQ_CASCADE         (2)
#define IRQ_COM2            (3)
#define IRQ_COM1            (4)
#define IRQ_LPT2            (5)
#define IRQ_FLOPPY          (6)
#define IRQ_LPT1            (7)
#define IRQ_CMOS            (8)
#define IRQ_PERIPHERALS_1   (9)
#define IRQ_PERIPHERALS_2   (10)
#define IRQ_PERIPHERALS_3   (11)
#define IRQ_PS2_MOUSE       (12)
#define IRQ_COPROCESSOR     (13)
#define IRQ_PRIMARY_ATA     (14)
#define IRQ_SECONDARY_ATA   (15)

/**
 * The IRQ handlers
 */
extern interrupt_handler_f irq_handlers[16];

/**
 * Will innitialize the irq handlers
 */
void irq_init();

///-------------------------------------------------------------------------
// Interrupt Request handlers, these will call the common interrupt handler
///-------------------------------------------------------------------------

extern void irq_pit();
extern void irq_keyboard();
extern void irq_cascade();
extern void irq_com2();
extern void irq_com1();
extern void irq_lpt2();
extern void irq_floppy();
extern void irq_lpt1();
extern void irq_cmos();
extern void irq_peripherals_1();
extern void irq_peripherals_2();
extern void irq_peripherals_3();
extern void irq_ps2_mouse();
extern void irq_coprocessor();
extern void irq_primary_ata();
extern void irq_secondary_ata();

#endif
