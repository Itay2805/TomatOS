#include "except.h"
#include "debug.h"

static const char* g_error_to_name[] = {
    [NO_ERROR] = "No error",
    [ERROR_CHECK_FAILED] = "Check failed",
    [ERROR_OUT_OF_RESOURCES] = "Out of resources",
    [ERROR_INVALID_HANDLE] = "Invalid handle",
    [ERROR_NOT_READY] = "Not ready",
    [ERROR_NOT_FOUND] = "Not found",
};

void _putchar(char character) {
    debug_write_char(character);
}

const char* strerror(err_t err) {
    return g_error_to_name[err];
}

