#ifndef TOMATKERNEL_HPET_H
#define TOMATKERNEL_HPET_H

#include <libc/assert.h>
#include <common/error.h>

/**
 * Will check if we can initialize the HPET driver
 */
bool hpet_is_supported();

/**
 * Will initialize the HPET timer driver
 */
error_t hpet_init();

#endif //TOMATKERNEL_HPET_H
