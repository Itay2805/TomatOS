#include <drivers/portio.h>
#include "pic.h"

error_t pic8259_disable() {
    log_debug("Disabling PIC8259");
    outb(0xa1, 0xff);
    outb(0x21, 0xff);
    return NO_ERROR;
}
