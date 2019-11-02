#ifndef TOMATKERNEL_APIC_H
#define TOMATKERNEL_APIC_H

#include <libc/stdint.h>

/**
 * Initialize the APIC
 */
void apic_init();

/**
 * Will return the processor id based on the lapic id
 */
uint64_t apic_get_processor_id();

#endif //TOMATKERNEL_APIC_H
