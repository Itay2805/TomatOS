#include <drivers/portio.h>
#include "e9hack.h"

error_t e9hack_write(const char* str) {
    while(*str) {
        outb(0xE9, *str++);
    }
    return NO_ERROR;
}

static void e9_log_wrapper(const char* str) {
    e9hack_write(str);
}

void e9hack_register_logger() {
    logger_t logger = (logger_t) {
        .write = e9_log_wrapper,
    };
    logger_register(&logger);

    log_info("E9 logger started!");
}
