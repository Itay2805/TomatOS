#ifndef TOMATKERNEL_KERNEL_DEBUGGER_H
#define TOMATKERNEL_KERNEL_DEBUGGER_H

/**
 * Print something to the debugger
 */
void debug_log(const char* fmt, ...);

// TODO: Have this shutdown all cores and shit
#define ASSERT(expr) \
    do { \
        if(!(expr)) { \
            debug_log("[-] Assertion failed at %s:%d!", __FILENAME__, __LINE__); \
            while(1); \
        } \
    } while(0)

#endif //TOMATKERNEL_KERNEL_DEBUGGER_H
