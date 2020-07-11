
#include "intrin.h"

/**
 * For amd64 we are going to simply use the e9 port
 */
void debug_write_char(char v) {
    __outb(0xE9, v);
}
