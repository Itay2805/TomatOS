#ifndef TOMATKERNEL_DEBUG_H
#define TOMATKERNEL_DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
};
#endif


#endif //TOMATKERNEL_DEBUG_H
