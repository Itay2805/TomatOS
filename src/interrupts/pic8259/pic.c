#include <helpers/portio.h>
#include <acpi/tables/madt.h>
#include "pic.h"

#define PIC_MASTER_COMMAND  0x20
#define PIC_MASTER_DATA     0x21
#define PIC_SLAVE_COMMAND   0xA0
#define PIC_SLAVE_DATA      0xA1

bool pic8259_available() {
    return (madt->flags & MADT_FLAGS_PIC8259) != 0;
}

error_t pic8259_disable() {
    outb(PIC_MASTER_DATA, 0xff);
    outb(PIC_SLAVE_DATA, 0xff);
    return NO_ERROR;
}
