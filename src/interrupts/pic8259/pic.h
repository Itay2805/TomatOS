#ifndef TOMATKERNEL_PIC_H
#define TOMATKERNEL_PIC_H

#include <error.h>

/**
 * Will check using ACPI if the pic is available for use
 *
 * @remark
 * ACPI table initialization must be done before this
 */
bool pic8259_available();

/**
 * Will disable the legacy pic
 */
error_t pic8259_disable();

#endif //TOMATKERNEL_PIC_H
