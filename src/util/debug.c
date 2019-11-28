#include <stb/stb_sprintf.h>
#include <drivers/terminal/terminal.h>

#include "arch.h"

#include "debug.h"
#include "sync.h"

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

static lock_t lock;

void debug_log(const char* fmt, ...) {
    acquire_lock(&lock);

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

#ifdef TERM_LOGGER
        terminal_write_char(*b);
#endif

        b++;
    }

    release_lock(&lock);
}
