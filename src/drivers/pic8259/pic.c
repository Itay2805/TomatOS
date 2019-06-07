#include <drivers/portio.h>
#include "pic.h"

#define PIC_MASTER_COMMAND  0x20
#define PIC_MASTER_DATA     0x21
#define PIC_SLAVE_COMMAND   0xA0
#define PIC_SLAVE_DATA      0xA1

#define PIC_COMMAND_EOF     0x20

#define PIC_INIT            0x11

static uint8_t master_offset;
static uint8_t slave_offset;

error_t pic8259_remap(uint8_t master, uint8_t slave) {
    error_t err = NO_ERROR;

    master_offset = master;
    slave_offset = slave;

    outb(PIC_MASTER_COMMAND, PIC_INIT);
    outb(PIC_SLAVE_COMMAND, PIC_INIT);

    outb(PIC_MASTER_DATA, master_offset);
    outb(PIC_SLAVE_DATA, slave_offset);

    outb(PIC_MASTER_DATA, 4);
    outb(PIC_SLAVE_DATA, 2);

    outb(PIC_MASTER_DATA, 1);
    outb(PIC_SLAVE_DATA, 1);

    return err;
}

error_t pic8259_eof(uint8_t irq) {
    if(irq >= slave_offset) {
        outb(PIC_SLAVE_COMMAND, PIC_COMMAND_EOF);
    }
    outb(PIC_MASTER_COMMAND, PIC_COMMAND_EOF);

    return NO_ERROR;
}

error_t pic8259_disable() {
    log_debug("Disabling PIC8259");
    outb(PIC_MASTER_DATA, 0xff);
    outb(PIC_SLAVE_DATA, 0xff);
    return NO_ERROR;
}
