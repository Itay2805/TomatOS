#include <arch/io.hpp>

#include "debug.hpp"

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

static void debug_write_char(char c) {
#ifdef E9_LOGGER
    e9_write_char(c);
#endif

#ifdef VBOX_LOGGER
    vbox_write_char(c);
#endif
}

/////////////////////////////////////////////////
// frigg logging and panic
/////////////////////////////////////////////////

extern "C" void frg_log(const char* cstring) {
    while(*cstring) {
        debug_write_char(*cstring++);
    }
    debug_write_char('\n');
}

extern "C" void frg_panic(const char* cstring) {
    while(*cstring) {
        debug_write_char(*cstring++);
    }
    // TODO: Panic to all cores
    while(1);
}

/////////////////////////////////////////////////
// TomatOS logs stuff
/////////////////////////////////////////////////

namespace util {

    // just use the frg_log
    _Alignas(alignof(frg_logger)) uint8_t log_buffer[sizeof(frg_logger)]{0};
    frg_logger* logger = nullptr;

    void init_debug_logger() {
        logger = new (log_buffer) frg_logger(frg_log);
    }

}
