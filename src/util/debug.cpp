#include <stb/stb_sprintf.h>
#include <arch/io.hpp>

#include "debug.h"

/////////////////////////////////////////////////
// Support logging functions
/////////////////////////////////////////////////

#ifdef E9_LOGGER
static void e9_write_char(char c) {
    arch::io::write8(0xE9, (uint8_t) c);
}
#endif

#ifdef VBOX_LOGGER
static void vbox_write_char(char c) {
    arch::io::write8(0x504, (uint8_t) c);
}
#endif

/////////////////////////////////////////////////
// The logging itself
/////////////////////////////////////////////////

extern "C" void debug_log(const char* fmt, ...) {
    // TODO: add back the lock stuff

    char buffer[128];
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