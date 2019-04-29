#include "except.h"

error_frame_t error_frames[EXCEPT_MAX_FRAMES];

const char* except_strings[] = {
        "No error",
        "Check failed",
        "Invalid argument",
        "Invalid pointer",
        "Already freed",
        "Out of memory"
};
