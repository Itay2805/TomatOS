#include <stb/stb_sprintf.h>

#include "arch.h"

#include "debug.h"

/////////////////////////////////////////////////
// Support logging functions
/////////////////////////////////////////////////

#ifdef E9_LOGGER
static void e9_write_char(char c) {
    io_write_8(0xE9, (uint8_t) c);
}
#endif

#ifdef VBOX_LOGGER
static void vbox_write_char(char c) {
    io_write_8(0x504, (uint8_t) c);
}
#endif

/////////////////////////////////////////////////
// The logging itself
/////////////////////////////////////////////////

static char buffer[512];

void debug_log(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    stbsp_vsnprintf(buffer, sizeof(buffer), fmt, ap);
    va_end(ap);

    char* b = buffer;
    while(*b) {
#ifdef E9_LOGGER
        e9_write_char(*b);
#endif

#ifdef VBOX_LOGGER
        vbox_write_char(*b);
#endif
        b++;
    }
}
