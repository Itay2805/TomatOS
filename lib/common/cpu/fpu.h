#ifndef TOMATKERNEL_FPU_H
#define TOMATKERNEL_FPU_H

#include <error.h>

/**
 * Save the fpu state
 *
 * @remark
 * Buffer must be 16byte aligned!
 *
 * @param buf   [OUT] The buffer to store the state in, must be 512bytes long
 */
static inline error_t _fxsave(uint8_t* buf) {
    error_t err = NO_ERROR;

    CHECK(ALIGN_DOWN(buf, 16) == (uint64_t) buf);
    asm volatile("fxsave (%0)" :: "r"(buf));

cleanup:
    return err;
}

/**
 * Restore the fpu state
 *
 * @remark
 * Buffer must be 16byte aligned!
 *
 * @param buf   [IN] The state to restore, must be 512bytes long
 */
static inline error_t _fxrstor(uint8_t* buf) {
    error_t err = NO_ERROR;

    CHECK(ALIGN_DOWN(buf, 16) == (uint64_t) buf);
    asm volatile("fxrstor (%0)" :: "r"(buf));

cleanup:
    return err;
}

#endif //TOMATKERNEL_FPU_H
