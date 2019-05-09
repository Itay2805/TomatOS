#include "except.h"

error_frame_t global_error_frames[EXCEPT_MAX_FRAMES];

const char* except_strings[] = {
        [NO_ERROR] = "No error",
        [ERROR_CHECK_FAILED] = "Check failed",
        [ERROR_INVALID_ARGUMENT] = "Invalid argument",
        [ERROR_INVALID_POINTER] = "Invalid pointer",
        [ERROR_ALREADY_FREED] = "Already freed",
        [ERROR_OUT_OF_MEMORY] = "Out of memory",
        [ERROR_NOT_FOUND] = "Not found",
        [ERROR_INVALID_SYSCALL] = "Invalid syscall",
        [ERROR_NOT_IMPLEMENTED] = "Not implemented",
        [ERROR_INVALID_DOMAIN] = "Invalid Domain",
        [ERROR_INVALID_PORT] = "Invalid port",
        [ERROR_INVALID_RESOURCE] = "Invalid resource",
        [ERROR_INVALID_PATH] = "Invalid path",
        [ERROR_NOT_READABLE] = "Not readable",
        [ERROR_NOT_WRITEABLE] = "Not writeable",
        [ERROR_NOT_SEEKABLE] = "Not seekable",
        [ERROR_OUT_OF_RANGE] = "Out of range",
        [ERROR_FINISHED] = "Finished",
};
