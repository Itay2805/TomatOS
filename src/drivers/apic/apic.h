#ifndef TOMATKERNEL_APIC_H
#define TOMATKERNEL_APIC_H

#include <error.h>

/**
 * Initialize the APIC
 *
 * @remark
 * Will not disable the Legacy PIC
 */
error_t apic_init();

/**
 * Will return the processor id based on the lapic id
 */
uint64_t apic_get_processor_id();

#endif //TOMATKERNEL_APIC_H
