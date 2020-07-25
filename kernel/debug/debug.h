#ifndef __TOMATOS_KERNEL_UTIL_DEBUG_H__
#define __TOMATOS_KERNEL_UTIL_DEBUG_H__

/**
 * Write to a debug output provided
 * by the architecture
 */
void debug_write_char(char v);

/**
 * Print a stack trace
 */
void debug_trace_stack(void* frame_pointer);

/**
 * Trace the caller's stack
 */
void debug_trace_own_stack();

#endif //__TOMATOS_KERNEL_UTIL_DEBUG_H__
