#ifndef TOMATKERNEL_PIC_H
#define TOMATKERNEL_PIC_H

#include <error.h>

/**
 * Remap the IRQs of the master and slave to a different offset
 *
 * @param master_offset [IN] Offset of the master (common value is 32)
 * @param slave_offset  [IN] Offset of the slave (common value is 40)
 */
error_t pic8259_remap(uint8_t master_offset, uint8_t slave_offset);

/**
 * Send an end of interrupt for the given interrupt
 */
error_t pic8259_eof(uint8_t irq);

/**
 * Will disable the legacy pic
 */
error_t pic8259_disable();

#endif //TOMATKERNEL_PIC_H
