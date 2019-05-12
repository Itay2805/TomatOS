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

static void ata_str_swap(char* str, int len) {
    for(int i = 0; i < len / 2; i++) {
        char tmp = str[i * 2];
        str[i * 2] = str[i * 2 + 1];
        str[i * 2 + 1] = tmp;
    }
}

static error_t ata_busy_wait(int channel, bool data) {
    error_t err = NO_ERROR;

    uint16_t iobase = portiop[channel];
    uint8_t status_raw = 0;
    ata_status_t* status = (ata_status_t *) &status_raw;
    uint64_t start = rtc_unixtime();
    do {
        status_raw = inb(iobase + ATA_REG_STATUS);
        CHECK(!status->error);
        CHECK_ERROR(rtc_unixtime() - start <= 1, ERROR_TIMEOUT);
    } while(status->drive_busy && (data ? status->data_request_ready : true));

cleanup:
    return err;
}

static const char* ata_error_strings[] = {
        "No address mark",
        "Track 0 not found",
        "Command aborted",
        "Media change request",
        "ID mark not found",
        "Media changed",
        "Uncorrectable data",
        "Bad block"
};

error_t ata_identify(int channel, int bus, ata_identify_t* identify) {
    error_t err = NO_ERROR;
    uint16_t iobase = portiop[channel];

    CHECK_ERROR(inb(iobase + ATA_REG_STATUS) != 0xFF, ERROR_NOT_FOUND);

    CHECK_ERROR(identify != NULL, ERROR_INVALID_ARGUMENT);

    // set the drive
    outb(iobase + ATA_REG_HARD_DRIVE_DEVICE_SELECT, port_to_device_port[bus]);
    outb(iobase + ATA_REG_SECTOR_COUNT_0, 0);
    outb(iobase + ATA_REG_LBA0, 0);
    outb(iobase + ATA_REG_LBA1, 0);
    outb(iobase + ATA_REG_LBA2, 0);

    // identify
    outb(iobase + ATA_REG_COMMAND, ATA_COMMAND_IDENTIFY);
    CHECK_AND_RETHROW(ata_busy_wait(channel, false));

    CHECK_ERROR(inb(iobase + ATA_REG_STATUS) != 0, ERROR_NOT_FOUND);
    CHECK_AND_RETHROW(ata_busy_wait(channel, false));

    // check if an ata drive
    CHECK_ERROR(inb(iobase + ATA_REG_LBA1) == 0 && inb(iobase + ATA_REG_LBA2) == 0, ERROR_NOT_FOUND);
    CHECK_AND_RETHROW(ata_busy_wait(channel, true));

    // TODO: use insw
    uint16_t* buffer = (uint16_t*)identify;
    for(int i = 0; i < 256; i++) {
        *buffer++ = inw(iobase + ATA_REG_DATA);
    }

    ata_str_swap(identify->model_number, 40);
    ata_str_swap(identify->serial_number, 20);
    ata_str_swap(identify->firmware_revision, 8);

cleanup:
    if(IS_ERROR(err) && IS_ERROR(err) != ERROR_NOT_FOUND) {
        uint8_t error = inb(iobase + ATA_REG_ERROR);
        bool printed = false;
        for(int i = 0; i < 8; i++) {
            if((error & (i << 1)) == 1) {
                LOG_ERROR("%s", ata_error_strings[i]);
                printed = true;
            }
        }
        if(!printed) {
            LOG_ERROR("Unknown error");
        }
    }
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
