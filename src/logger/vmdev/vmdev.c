#include <drivers/portio.h>
#include "vmdev.h"

#define BOCHS_LOG_PORT  0xE9
#define VBOX_LOG_PORT   0x504

static bool log_e9 = false;

error_t vmdev_write(const char *str) {
    while(*str) {
        if(log_e9 ) {
            // Bochs/Qemu
            outb(BOCHS_LOG_PORT, (uint8_t) *str);
        }

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
    .enabled = true,
};

void vmdev_register_logger() {
    logger_register(&logger);

    if(inb(BOCHS_LOG_PORT) == 0xe9) {
        log_e9 = true;
    }

    log_info("VM logger started! (e9=%s)", log_e9 ? "true" : "false");
}
