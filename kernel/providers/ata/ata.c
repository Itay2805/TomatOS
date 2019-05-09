#include <drivers/portio.h>
#include "ata.h"

#define MASTER	0xE0
#define SLAVE	0xF0

#define MASTER_IDENTIFIER	0xA0
#define SLAVE_IDENTIFIER	0xB0

#define CURRENT_IDENTIFIER	MASTER_IDENTIFIER
#define CURRENT				MASTER

#define PRIMARY_ATA_PORT_BASE	(uint8_t)0x1F0
#define SECONDARY_ATA_PORT_BASE     (uint8_t)0x170

#define DATA_PORT				(uint16_t)0
#define ERROR_PORT				(uint16_t)1
#define SECTOR_COUNT_PORT		(uint16_t)2
#define LBA_LOW_PORT			(uint16_t)3
#define LBA_MID_PORT			(uint16_t)4
#define LBA_HI_PORT				(uint16_t)5
#define DEVICE_PORT				(uint16_t)6
#define COMMAND_PORT			(uint16_t)7
#define CONTROL_PORT			(uint16_t)518

static uint8_t port_to_device_port[] = {
    [0] = MASTER_IDENTIFIER,
    [1] = SLAVE_IDENTIFIER
};

static uint8_t  cintrooler_to_device_controller[] = {
    [0] = MASTER,
    [1] = SLAVE
};

static uint16_t portiop[] = {
    [0] = PRIMARY_ATA_PORT_BASE,
    [1] = SECONDARY_ATA_PORT_BASE
};

bool ata_identify(int drive, int port, ata_identify_t* identify) {
    uint16_t iobase = portiop[drive];

    // set the drive
    outb(iobase + DEVICE_PORT, port_to_device_port[port]);
    outb(iobase + CONTROL_PORT, 0);
    outb(iobase + DEVICE_PORT, port_to_device_port[port]);

    // get the status
    uint8_t status = inb(iobase + COMMAND_PORT);
    if(status == 0xFF) {
        return false;
    }

    // send the identify command
    outb(iobase + DEVICE_PORT, port_to_device_port[port]);
    outb(iobase + SECTOR_COUNT_PORT, 0);
    outb(iobase + LBA_LOW_PORT, 0);
    outb(iobase + LBA_MID_PORT, 0);
    outb(iobase + LBA_HI_PORT, 0);
    outb(iobase + COMMAND_PORT, 0xEC);
    status = inb(iobase + COMMAND_PORT);
    if(status == 0) {
        return false;
    }

    // wait for the drive to be ready
    do {
        status = inb(iobase + COMMAND_PORT);
    } while (((status & 0x80) == 0x80) && (status & 0x1) != 0x01);

    // check if we had an error
    if(status & 0x1) {
        return false;
    }

    uint16_t* buffer = (uint16_t*)identify;
    for(int i = 0; i < 256; i++) {
        *buffer++ = inw(iobase + DATA_PORT);
    }

    return true;
}

void ata_write(int drive, int port, size_t sector, uintptr_t buffer, int count) {

}

void ata_read(int drive, int port, size_t sector, uintptr_t buffer, int count) {

}

void ata_flush(int drive, int port) {

}
