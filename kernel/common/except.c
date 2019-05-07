#include "except.h"

error_frame_t error_frames[EXCEPT_MAX_FRAMES];

const char* except_strings[] = {
        [NO_ERROR] = "No error",
        [ERROR_CHECK_FAILED] = "Check failed",
        [ERROR_INVALID_ARGUMENT] = "Invalid argument",
        [ERROR_INVALID_POINTER] = "Invalid pointer",
        [ERROR_ALREADY_FREED] = "Already freed",
        [ERROR_OUT_OF_MEMORY] = "Out of memory",
        [ERROR_NOT_FOUND] = "Not found",
        [ERROR_INVALID_SYSCALL] = "Invalid syscall",
        [ERROR_NOT_IMPLEMENTED] = "Not implemented"
};
