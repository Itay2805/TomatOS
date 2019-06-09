#ifndef TOMATKERNEL_FPU_H
#define TOMATKERNEL_FPU_H

#include <error.h>

/**
 * Will initialize the fpu and enable whatever we can
 */
error_t fpu_init();

// TODO: Abstraction of the fpu state maybe

/**
 * Save the fpu state
 *
 * @remark
 * Buffer must be 16byte aligned!
 *
 * @param buf   [OUT] The buffer to store the state in, must be 512bytes long
 */
error_t _fxsave(uint8_t* buf);

/**
 * Restore the fpu state
 *
 * @remark
 * Buffer must be 16byte aligned!
 *
 * @param buf   [IN] The state to restore, must be 512bytes long
 */
error_t _fxrstor(uint8_t* buf);

#endif //TOMATKERNEL_FPU_H
