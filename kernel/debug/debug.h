#ifndef TOMATOS_DEBUG_H
#define TOMATOS_DEBUG_H

/**
 * Write a char to the debug output
 */
void debug_write_char(char c);

/**
 * Read a char from debug input
 * returns -1 if no debug input is connected
 */
int debug_read_char();

/**
 * Print a stack trace
 */
void debug_trace_stack(void* frame_pointer);

/**
 * Trace the caller's stack
 */
static inline void debug_trace_me() {
    debug_trace_stack(__builtin_frame_address(0));
}

#endif //TOMATOS_DEBUG_H
