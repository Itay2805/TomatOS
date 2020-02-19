#ifndef __INTR_APIC_LAPIC_H__
#define __INTR_APIC_LAPIC_H__

#include <stddef.h>
#include <stdint.h>

/**
 * Retrieve the base address of local APIC.
 */
uintptr_t get_local_apic_base_address();

/**
 * Get the local APIC ID of the executing processor.
 */
size_t get_apic_id();

/**
 * Send a Fixed IPI to a specified target processor.
 *
 * This function returns after the IPI has been accepted by the target processor.
 *
 * @param apic_id   [IN] The local APIC ID of the target processor.
 * @param vector    [IN] The vector number of the interrupt being sent.
 */
void send_fixed_ipi(uint32_t apic_id, uint8_t vector);

/**
 * Initialize the state of the SoftwareEnable bit in the Local APIC
 * Spurious Interrupt Vector register.
 */
void lapic_init();

/**
 * Send EOI to the local APIC.
 */
void send_apic_eoi();

#endif //__INTR_APIC_LAPIC_H__
