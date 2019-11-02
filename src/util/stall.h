#ifndef TOMATKERNEL_STALL_H
#define TOMATKERNEL_STALL_H

#include <tboot.h>

/**
 * Init the stall
 *
 * @param info  [IN] The boot info
 */
void stall_init(tboot_info_t* info);

/**
 * Will stop the processor for a couple of millisecond
 *
 * @remark
 * This will not touch the interrupts state
 *
 * @param milliseconds  [IN] The time to stall
 */
void stall(uint64_t milliseconds);

#endif //TOMATKERNEL_STALL_H
