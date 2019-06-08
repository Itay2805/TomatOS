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

#endif //TOMATKERNEL_APIC_H
