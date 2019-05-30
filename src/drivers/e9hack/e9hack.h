#include <error.h>

#ifndef TOMATKERNEL_E9HACK_H
#define TOMATKERNEL_E9HACK_H

/**
 * Will write to the e9 port, which is used by qemu and bochs as a
 * quick hack for the guest to write to the console.
 *
 * useful for early logging before we initialize all of the kernel.
 *
 * @remark
 * This is pretty slow.
 *
 * @see
 * https://wiki.osdev.org/Bochs#I.2FO_debugger_macros
 *
 * @param str [IN] The string to print to E9
 * @return NO_ERROR: Success
 */
error_t e9hack_write(const char* str);

/**
 * Register e9 as a logger output
 */
void e9hack_register_logger();

#endif //TOMATKERNEL_E9HACK_H
