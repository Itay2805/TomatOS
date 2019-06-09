#ifndef TOMATKERNEL_FPU_H
#define TOMATKERNEL_FPU_H

#include <string.h>

/**
 * Save the fpu state
 *
 * @param buf   [OUT] The buffer to store the state in, must be 512bytes long
 */
static inline void _fxsave(char* buf) {
    char aligned_buffer[512]__attribute__((aligned(16)));
    asm volatile("fxsave %0" :: "m"(aligned_buffer));
    memcpy(buf, aligned_buffer, 512);
}

/**
 * Restore the fpu state
 *
 * @param buf   [IN] The state to restore, must be 512bytes long
 */
static inline void _fxrstor(void* buf) {
    char aligned_buffer[512]__attribute__((aligned(16)));
    memcpy(aligned_buffer, buf, 512);
    asm volatile("fxrstor %0" :: "m"(aligned_buffer));
}

#endif //TOMATKERNEL_FPU_H
