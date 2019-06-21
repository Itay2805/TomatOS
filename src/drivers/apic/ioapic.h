#ifndef TOMATKERNEL_IOAPIC_H
#define TOMATKERNEL_IOAPIC_H

#include <drivers/acpi/tables/madt.h>
#include <error.h>

#define IOAPIC_REG_ID                           0x00
#define IOAPIC_REG_VERSION                      0x01
#define IOAPIC_REG_ARBITRATION_ID               0x02
#define IOAPIC_REG_REDIRECTION_TABLE_BASE       0x10

#define IOAPIC_DELIVERY_MODE_FIXED              0
#define IOAPIC_DELIVERY_MODE_LOWEST_PRIORITY    1
#define IOAPIC_DELIVERY_MODE_SMI                2
#define IOAPIC_DELIVERY_MODE_NMI                4
#define IOAPIC_DELIVERY_MODE_INIT               5
#define IOAPIC_DELIVERY_MODE_EXTINT             7

typedef union ioapic_redirection_entry {
    struct {
        uint64_t vector : 8;
        uint64_t delievery_mode : 3;
        uint64_t destination_mode : 1;
        uint64_t delivery_status : 1;
        uint64_t polarity : 1;
        uint64_t remote_irr : 1;
        uint64_t trigger_mode : 1;
        uint64_t mask : 1;
        uint64_t _reserved : 39;
        uint64_t destination_id : 8;
    } __attribute__((packed));
    struct {
        uint32_t raw_low;
        uint32_t raw_high;
    } __attribute__((packed));
    uint64_t raw;
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
void ioapic_write(madt_ioapic_t* ioapic, uint32_t reg, uint32_t value);

/**
 * Read from a ioapic register
 *
 * @param reg       [IN] The register
 */
uint32_t ioapic_read(madt_ioapic_t* ioapic, uint32_t reg);

/**
 * Will return the range of GSIs that this IOAPIC handles
 */
void ioapic_get_range(madt_ioapic_t* ioapic, uint32_t* gsi_start, uint32_t* gsi_end);

/**
 * Get the IOAPIC from the gsi
 */
madt_ioapic_t* ioapic_get_from_gsi(uint32_t gsi);

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
