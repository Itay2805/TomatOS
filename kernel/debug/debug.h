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
static inline void debug_trace_own_stack() {
    debug_trace_stack(__builtin_frame_address(0));
}

#endif //__TOMATOS_KERNEL_UTIL_DEBUG_H__
