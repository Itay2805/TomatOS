#include <drivers/portio.h>
#include "vmdev.h"

error_t vmdev_write(const char *str) {
    while(*str) {
        // Bochs/Qemu
        outb(0xE9, (uint8_t) *str);

        // VirtualBox
        outb(0x504, (uint8_t) *str);

        str++;
    }
    return NO_ERROR;
}

static void e9_log_wrapper(const char* str) {
    vmdev_write(str);
}

static logger_t logger = (logger_t) {
    .write = e9_log_wrapper,
};

void vmdev_register_logger() {
    logger_register(&logger);

    log_info("E9 logger started!");
}
