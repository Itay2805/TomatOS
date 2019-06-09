#ifndef TOMATKERNEL_IOAPIC_H
#define TOMATKERNEL_IOAPIC_H

#include <error.h>

#define IOAPIC_REG_ID                           0x00
#define IOAPIC_REG_VERSION                      0x01
#define IOAPIC_REG_ARBITRATION_ID               0x02
#define IOAPIC_REG_REDIRECTION_TABLE_BASE       0x10

#define LAPIC_DELIEVERY_MODE_FIXED              0b000
#define LAPIC_DELIEVERY_MODE_LOWEST_PRIORITY    0b001
#define LAPIC_DELIEVERY_MODE_SMI                0b010
#define LAPIC_DELIEVERY_MODE_NMI                0b100
#define LAPIC_DELIEVERY_MODE_INIT               0b101
#define LAPIC_DELIEVERY_MODE_ExtINT             0b111

typedef union ioapic_redirection_entry {
    struct {
        uint64_t vector : 8;
        uint64_t delievery_mode : 3;
        uint64_t logical_mode : 1;
        uint64_t pending : 1;
        uint64_t low_active : 1;
        uint64_t remote_irr : 1;
        uint64_t edge_sensitive : 1;
        uint64_t masked : 1;
        uint64_t _reserved : 39;
        uint64_t destination : 8;
    } __attribute__((packed));
    struct {
        uint32_t raw_low;
        uint32_t raw_high;
    } __attribute__((packed));
} ioapic_redirection_entry_t;

/**
 * Initialize the I/O APIC
 * @return
 */
error_t ioapic_init();

/**
 * Write into am ioapic register
 *
 * @param reg       [IN] The register
 * @param value     [IN] The value
 */
void ioapic_write(uint32_t reg, uint32_t value);

/**
 * Read from a ioapic register
 *
 * @param reg       [IN] The register
 */
uint32_t ioapic_read(uint32_t reg);

/**
 * Redirect the given IRQ to a vector
 *
 * @remark
 * The vector will be added the interrupt base just so we
 * won't overlap with the rest of the exceptions/interrupts
 *
 * @param irq       [IN] The
 * @param vector    [IN]
 */
error_t ioapic_redirect(uint32_t irq, uint8_t vector);

#endif //TOMATKERNEL_IOAPIC_H
