#ifndef TOMATKERNEL_IRQ_H
#define TOMATKERNEL_IRQ_H

#include <error.h>
#include "interrupts.h"

/**
 * Initialize the irq handlers
 *
 * @remark
 * This will be called by the idt_init function, do not call this manually
 */
void irq_init();

/**
 * Set an handler for an irq
 *
 * @param handler   [IN]
 */
error_t irq_set_handler(uint32_t irq, interrupt_handler_f handler);

#endif //TOMATKERNEL_IRQ_H
