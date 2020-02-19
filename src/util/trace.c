#include <stb_sprintf.h>
#include <arch/intrin.h>
#include <event/event.h>

#include <stdarg.h>

#include "serial.h"
#include "trace.h"

static char* stb_print_callback(const char* buf, void* user, int len) {
    // send to serial
    serial_write(buf, len);

    // send to port 0xE9/0x504 (Qemu/VBox log port)
    char* ret = (char*)buf;
    while (len--) {
        __outbyte(0xE9, *buf);
        __outbyte(0x504, *buf);
        buf++;
    }
    return ret;
}

void trace(const char* fmt, ...) {
    tpl_t tpl = raise_tpl(TPL_HIGH_LEVEL);

    char buffer[STB_SPRINTF_MIN] = {0};
    va_list list;
    va_start(list, fmt);
    stbsp_vsprintfcb(stb_print_callback, NULL, buffer, fmt, list);
    va_end(list);

    restore_tpl(tpl);
}
