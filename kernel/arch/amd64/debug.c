
#include <stddef.h>
#include <util/except.h>
#include <debug/early_console.h>
#include "intrin.h"

/**
 * For amd64 we are going to simply use the e9 port
 */
void debug_write_char(char v) {
#ifdef __TOMATOS_QEMU_CONSOLE__
    __outb(0xe9, v);
#endif
#ifdef __TOMATOS_EARLY_CONSOLE__
    early_console_putc(v);
#endif
}


typedef struct frame {
    struct frame* rbp;
    uint64_t rip;
} frame_t;

void debug_trace_stack(void* frame_pointer) {
    frame_t* current = frame_pointer;
    for (size_t i = 0; i < UINT64_MAX; i++) {
        if(!current) {
            break;
        }

        TRACE("\t%02d: RIP [%p]", i, current->rip);

        current = current->rbp;
    }
}

