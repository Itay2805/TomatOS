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
        "Not mapped",
        "Invalid scheme",
        "Invalid domain",
        "Invalid port",
        "Scheme already exists",
        "Domain already exists",
        "Port already exists",
        "Path already exists",
        "Not readable",
        "Not writeable",
        "Not seekable",
        "Not a directory",
        "Not pollable",
        "Invalid resource"
};

