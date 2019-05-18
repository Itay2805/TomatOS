#include <common/stdarg.h>
#include <common/mini-printf.h>
#include "qemu.h"
#include "portio.h"

void qemu_write(const char* str) {
    while(*str) {
        outb(0xE9, (uint8_t) *str++);
    }
}

void qemu_print(const char* fmt, ...) {
    va_list list;
    va_start(list, fmt);
    char buffer[1024];
    mini_vsnprintf(buffer, 1024u, fmt, list);
    qemu_write(buffer);
    va_end(list);
}