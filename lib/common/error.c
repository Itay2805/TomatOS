#include "error.h"

#include <stddef.h>

const char* error_names[ERROR_COUNT] = {
        [NO_ERROR] = "No error",
        [ERROR_CHECK_FAILED] = "Check failed",
        [ERROR_OUT_OF_MEMORY] = "Out of memory",

        [ERROR_ALREADY_MAPPED] = "Already mapped",

        [ERROR_INVALID_RESOURCE] = "Invalid resource",
        [ERROR_INVALID_ARGUMENT] = "Invalid argument",
        [ERROR_INVALID_POINTER] = "Invalid pointer",
        [ERROR_INVALID_SCHEME] = "Invalid scheme",
        [ERROR_INVALID_DOMAIN] = "Invalid domain",
        [ERROR_INVALID_PORT] = "Invalid port",
        [ERROR_INVALID_PATH] = "Invalid path",

        [ERROR_SCHEME_EXISTS] = "Scheme already exists",
        [ERROR_DOMAIN_EXISTS] = "Domain already exists",
        [ERROR_PORT_EXISTS] = "Port already exists",
        [ERROR_PATH_EXISTS] = "Path already exists",

        [ERROR_NOT_IMPLEMENTED] = "Not implemented",
        [ERROR_NOT_DIRECTORY] = "Not a directory",
        [ERROR_NOT_WRITEABLE] = "Not writeable",
        [ERROR_NOT_SUPPORTED] = "Not supported",
        [ERROR_NOT_READABLE] = "Not readable",
        [ERROR_NOT_SEEKABLE] = "Not seekable",
        [ERROR_NOT_POLLABLE] = "Not pollable",
        [ERROR_NOT_MAPPED] = "Not mapped",
        [ERROR_NOT_FOUND] = "Not found",

        [ERROR_THREAD_CANCELED] = "Thread was canceled",
        [ERROR_THREAD_RUNNING] = "Thread is running",
};

