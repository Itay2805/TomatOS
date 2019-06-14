#include <drivers/portio.h>
#include "vmdev.h"

#define BOCHS_LOG_PORT  0xE9
#define VBOX_LOG_PORT   0x504

error_t vmdev_write(const char *str) {
    while(*str) {
        // Bochs/Qemu
        outb(BOCHS_LOG_PORT, (uint8_t) *str);

        // VirtualBox
        outb(VBOX_LOG_PORT, (uint8_t) *str);

        str++;
    }
    return NO_ERROR;
}

static void vmdev_log_wrapper(const char* str) {
    vmdev_write(str);
}

static logger_t logger = (logger_t) {
    .write = vmdev_log_wrapper,
};

void vmdev_register_logger() {
    logger_register(&logger);

    log_info("VM logger started!");
}
