#include <drivers/portio.h>
#include <common/except.h>
#include <common/string.h>
#include <cpu/msr.h>
#include <drivers/rtc.h>

#include "ata.h"

static uint8_t port_to_device_port[] = {
    [0] = 0xA0,
    [1] = 0xB0
};

static uint8_t  cintrooler_to_device_controller[] = {
    [0] = 0xE0,
    [1] = 0xF0
};

static uint16_t portiop[] = {
    [0] = 0x1F0,
    [1] = 0x170
};

error_t ata_identify(int drive, int port, ata_identify_t* identify) {
    error_t err = NO_ERROR;
    uint16_t iobase = portiop[drive];

    CHECK_ERROR(identify != NULL, ERROR_INVALID_ARGUMENT);

    // set the drive
    outb(iobase + ATA_REG_HARD_DRIVE_DEVICE_SELECT, port_to_device_port[port]);
    outb(iobase + ATA_REG_SECTOR_COUNT_0, 0);
    outb(iobase + ATA_REG_LBA0, 0);
    outb(iobase + ATA_REG_LBA1, 0);
    outb(iobase + ATA_REG_LBA2, 0);

    // identify
    outb(iobase + ATA_REG_COMMAND, ATA_COMMAND_IDENTIFY);
    CHECK_ERROR(inb(iobase + ATA_REG_STATUS) != 0, ERROR_NOT_FOUND);
    uint64_t timeout_start = rtc_unixtime();
    while(inb(iobase + ATA_REG_STATUS) & 0x80) {
        CHECK_ERROR(rtc_unixtime() - timeout_start < 1, ERROR_TIMEOUT);
    }

    // check if an ata drive
    CHECK_ERROR(inb(iobase + ATA_REG_LBA1) == 0 && inb(iobase + ATA_REG_LBA2) == 0, ERROR_NOT_FOUND);

    timeout_start = rtc_unixtime();
    while(true) {
        uint8_t status = inb(iobase + ATA_REG_STATUS);
        if(status & 8) break;
        CHECK((status & 1) == 0);
        CHECK_ERROR(rtc_unixtime() - timeout_start < 1, ERROR_TIMEOUT);
    }

    // TODO: use insw
    uint16_t* buffer = (uint16_t*)identify;
    for(int i = 0; i < 256; i++) {
        *buffer++ = inw(iobase + ATA_REG_DATA);
    }

cleanup:
    return err;
}

error_t ata_write_sector(int drive, int port, uint64_t lba, char* buffer) {
    uint16_t iobase = portiop[drive];

    // send the command and wait
    outb(iobase + ATA_REG_FEATURES, 0x00);
    outb(iobase + ATA_REG_SECTOR_COUNT_0, 1);
    outb(iobase + ATA_REG_HARD_DRIVE_DEVICE_SELECT,
            (uint8_t) (cintrooler_to_device_controller[drive] |
            (port_to_device_port[port] << 4) |
            ((lba & 0x0f000000) >> 24)));
    outb(iobase + ATA_REG_LBA0, (uint8_t) ((lba & 0x000000ff) >> 0));
    outb(iobase + ATA_REG_LBA1, (uint8_t) ((lba & 0x0000ff00) >> 8));
    outb(iobase + ATA_REG_LBA2, (uint8_t) ((lba & 0x00ff0000) >> 16));
    outb(iobase + ATA_REG_COMMAND, ATA_COMMAND_WRITE_PIO);
    while(inb(iobase + ATA_REG_STATUS) & 0x80);

    // write out the buffer
    uint16_t* buf = (uint16_t *)buffer;
    for(int i = 0; i < 256; i++) {
        outw(iobase + ATA_REG_DATA, *buf++);
    }
    outb(iobase + ATA_REG_COMMAND, ATA_COMMAND_CACHE_FLUSH);

    return NO_ERROR;
}

error_t ata_read_sector(int drive, int port, uint64_t lba, char* buffer) {
    uint16_t iobase = portiop[drive];

    outb(iobase + ATA_REG_FEATURES, 0x00);
    outb(iobase + ATA_REG_SECTOR_COUNT_0, 1);
    outb(iobase + ATA_REG_HARD_DRIVE_DEVICE_SELECT,
         (uint8_t) (cintrooler_to_device_controller[drive] |
                    (port_to_device_port[port] << 4) |
                    ((lba & 0x0f000000) >> 24)));
    outb(iobase + ATA_REG_LBA0, (uint8_t) ((lba & 0x000000ff) >> 0));
    outb(iobase + ATA_REG_LBA1, (uint8_t) ((lba & 0x0000ff00) >> 8));
    outb(iobase + ATA_REG_LBA2, (uint8_t) ((lba & 0x00ff0000) >> 16));
    outb(iobase + ATA_REG_COMMAND, ATA_COMMAND_READ_PIO);

    // write out the buffer
    uint16_t* buf = (uint16_t *)buffer;
    for(int i = 0; i < 256; i++) {
        while(inb(iobase + ATA_REG_STATUS) & 0x80);
        *buf++ = inw(iobase + ATA_REG_DATA);
    }

    return NO_ERROR;
}
