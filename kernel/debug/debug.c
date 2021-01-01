
#include <arch/io.h>
#include <sync/lock.h>
#include <util/trace.h>

void debug_write_char(char c) {
    io_write_8(0xE9, c);
}

int debug_read_char() {
    return -1;
}

void enter_debug_console() {
    DEBUG("Entering debug console");
    while(1);
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

        TRACE("\t", i,": RIP [", (void*)current->rip, "]");

        current = current->rbp;
    }
}
