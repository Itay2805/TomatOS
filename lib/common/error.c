#include "error.h"

#include <stddef.h>

const char* error_names[ERROR_COUNT] = {
        NULL,
        "Check failed",
        "Invalid argument",
        "Invalid pointer",
        "Out of memory",
        "Not found",
        "Already mapped",
        "Not mapped"
};

