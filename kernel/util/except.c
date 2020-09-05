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

void trace_hex(const void* data, size_t size) {
    char ascii[17];
    size_t i, j;
    ascii[16] = '\0';
    for (i = 0; i < size; ++i) {
        PRINT("%02X ", ((unsigned char*)data)[i]);
        if (((unsigned char*)data)[i] >= ' ' && ((unsigned char*)data)[i] <= '~') {
            ascii[i % 16] = ((unsigned char*)data)[i];
        } else {
            ascii[i % 16] = '.';
        }
        if ((i+1) % 8 == 0 || i+1 == size) {
            PRINT(" ");
            if ((i+1) % 16 == 0) {
                PRINT("|  %s \n", ascii);
            } else if (i+1 == size) {
                ascii[(i+1) % 16] = '\0';
                if ((i+1) % 16 <= 8) {
                    PRINT(" ");
                }
                for (j = (i+1) % 16; j < 16; ++j) {
                    PRINT("   ");
                }
                PRINT("|  %s \n", ascii);
            }
        }
    }
}

const char* strerror(err_t err) {
    return g_error_to_name[err];
}
