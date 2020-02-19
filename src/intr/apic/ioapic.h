#ifndef __INTR_APIC_IOAPIC_H__
#define __INTR_APIC_IOAPIC_H__

#include <acpi/tables/madt.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum ioapic_delivery_mode {
    IO_APIC_DELIVERY_MODE_FIXED,
    IO_APIC_DELIVERY_MODE_LOWEST_PRIORITY,
    IO_APIC_DELIVERY_MODE_SMI,
    IO_APIC_DELIVERY_MODE_NMI,
    IO_APIC_DELIVERY_MODE_INIT,
    IO_APIC_DELIVERY_MODE_EXTINT
} ioapic_delivery_mode_t;

/**
 * Add an I/O APIC to the driver
 */
void ioapic_add(uintptr_t mmio_base, size_t gsi_base);

/**
 * Add an Interrupt Source Override entry
 */
void ioapic_add_iso(madt_iso_t* iso);

/**
 * Set the interrupt mask of an I/O APIC interrupt.
 *
 * If Irq is larger than the maximum number I/O APIC redirection entries, then ASSERT().
 *
 * @param irq       [IN] Specifies the I/O APIC interrupt to enable or disable.
 * @param enable    [IN] true to enable the I/O APIC interrupt, false to disable
 */
void ioapic_enable_interrupt(size_t irq, bool enable);

/**
 * Configures an I/O APIC interrupt.
 *
 * Configure an I/O APIC Redirection Table Entry to deliver an interrupt in physical
 * mode to the Local APIC of the currently executing CPU.  The default state of the
 * entry is for the interrupt to be disabled (masked).  ioapic_enable_interrupt() must
 * be used to enable(unmask) the I/O APIC Interrupt.
 *
 * If Irq is larger than the maximum number I/O APIC redirection entries, then ASSERT().
 * If Vector >= 0x100, then ASSERT().
 *
 * @param irq               [IN] Specifies the I/O APIC interrupt to initialize.
 * @param vector            [IN] The 8-bit interrupt vector associated with the I/O APIC Interrupt.
 * @param delivery_mode     [IN] A 3-bit value that specifies how the recept of the I/O APIC interrupt is handled
 * @param level_triggered   [IN] true for level triggered and false for edge triggered
 * @param assertion_level   [IN] true for active high interrupt and false for active low interrupt
 */
void ioapic_configure_interrupt(size_t irq, size_t vector, ioapic_delivery_mode_t delivery_mode, bool level_triggered, bool assertion_level);

#endif //__INTR_APIC_IOAPIC_H__
