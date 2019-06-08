#ifndef TOMATKERNEL_LAPIC_H
#define TOMATKERNEL_LAPIC_H

#include <error.h>

// End Of Interrupt
#define LAPIC_REG_EOI           0xb0

// Spurious Interrupt Vector Register
#define LAPIC_REG_SIVR          0xf0

// Interrupt Control Register
#define LAPIC_REG_ICR1          0x300
#define LAPIC_REG_ICR2          0x310

// Local Vector Table Register
#define LAPIC_REG_LVTR_TIMER    0x320
#define LAPIC_REG_LVTR_LINT0    0x320
#define LAPIC_REG_LVTR_LINT1    0x360

typedef struct lapic_lvtr {
    union {
        struct {
            uint8_t vector : 8;
            uint8_t reserved_0 : 1;
            uint8_t nmi : 1;
            uint8_t reserved_1 : 1;
            uint8_t reserved_2 : 1;
            uint8_t interrupt_pending : 1;
            uint8_t low_triggered : 1;
            uint8_t remote_irr : 1;
            uint8_t level_triggered : 1;
            uint8_t masked : 1;
        };
        uint32_t raw;
    };
} __attribute__((packed)) lapic_lvtr_t;

/**
 * Initialize the Local APIC
 *
 * This will set the NMIs and Spurious interrupts
 * @return
 */
error_t lapic_init();

// TODO: Helper methods for the lint and icr interrupts?

/**
 * Send and End Of Interrupt to the local APIC
 *
 * @remark
 * This just writes 0 to the LAPIC_REG_EOI
 */
error_t lapic_eoi();

/**
 * Write into a lapic register
 *
 * @param reg       [IN] The register
 * @param value     [IN] The value
 */
void lapic_write(uint32_t reg, uint32_t value);

/**
 * Read from a lapic register
 *
 * @param reg       [IN] The register
 */
uint32_t lapic_read(uint32_t reg);

#endif //TOMATKERNEL_LAPIC_H
