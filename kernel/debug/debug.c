
#include <arch/io.h>
#include <sync/lock.h>

void debug_write_char(char c) {
    io_write_8(0xE9, c);
}

int debug_read_char() {
    return -1;
}
