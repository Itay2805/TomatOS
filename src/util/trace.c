#include <stb_sprintf.h>
#include <arch/intrin.h>
#include <event/event.h>

#include <stdarg.h>

#include "serial.h"
#include "trace.h"

void trace(const char* fmt, ...) {
    tpl_t tpl = raise_tpl(TPL_HIGH_LEVEL);
    char buffer[STB_SPRINTF_MIN] = {0};

    va_list list;
    va_start(list, fmt);
    size_t len = stbsp_vsnprintf(buffer, sizeof(buffer), fmt, list);
    va_end(list);

    // send to serial
    serial_write(buffer, len);

    // send to port 0xE9/0x504 (Qemu/VBox log port)
    for (int i = 0; i < len; i++) {
        __outbyte(0xE9, buffer[i]);
        __outbyte(0x504, buffer[i]);
    }

    restore_tpl(tpl);
}
