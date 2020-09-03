#include <sync/lock.h>
#include <sync/critical.h>
#include "except.h"
#include "debug/debug.h"

static const char* g_error_to_name[] = {
    [NO_ERROR] = "No error",
    [ERROR_CHECK_FAILED] = "Check failed",
    [ERROR_OUT_OF_RESOURCES] = "Out of resources",
    [ERROR_INVALID_HANDLE] = "Invalid handle",
    [ERROR_NOT_READY] = "Not ready",
    [ERROR_NOT_FOUND] = "Not found",
};

static ticket_lock_t g_debug_output_lock = INIT_LOCK();

void _putchar(char character) {
    debug_write_char(character);
}

void trace(const char* fmt, ...) {
    ticket_lock(&g_debug_output_lock);

    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);

    ticket_unlock(&g_debug_output_lock);
}

const char* strerror(err_t err) {
    return g_error_to_name[err];
}
