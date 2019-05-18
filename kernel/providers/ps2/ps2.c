#include <drivers/portio.h>
#include <locks/spinlock.h>
#include <common/except.h>
#include <common/string.h>

#include "ps2.h"

#define PS2_REG_DATA        0x60
#define PS2_REG_STATUS      0x64
#define PS2_REG_COMMAND     0x64

#define PS2_CMD_DISABLE_FIRST_PORT      0xAD
#define PS2_CMD_ENABLE_FIRST_PORT       0xAE
#define PS2_CMD_DISABLE_SECOND_PORT     0xA7
#define PS2_CMD_ENABLE_SECOND_PORT      0xA8
#define PS2_CMD_WRITE_SECOND_PORT       0xD4

typedef struct ps2_status {
    /*
     * Output buffer status
     * 0 = empty
     * 1 = full
     *
     * (must be set before attempting to read data from IO port 0x60)
     */
    uint8_t output_buffer : 1;

    /*
     * Input buffer status
     * 0 = empty
     * 1 = full
     *
     * (must be clear before attempting to write data to IO port 0x60 or IO port 0x64)
     */
    uint8_t input_buffer : 1;

    /*
     * System Flag
     *
     * Meant to be cleared on reset and set by firmware (via. PS/2 Controller Configuration Byte)
     * if the system passes self tests (POST)
     */
    uint8_t system_flag : 1;

    /*
     * Command/data
     * 0 = data written to input buffer is data for PS/2 device,
     * 1 = data written to input buffer is data for PS/2 controller command
     */
    uint8_t mode : 1;

    uint8_t unknown : 2;

    /*
     * Time-out error
     */
    uint8_t time_out_error : 1;

    /*
     * Parity error
     */
    uint8_t parity_error : 1;
} __attribute__((packed)) ps2_status_t;

static ps2_status_t ps2_get_status() {
    char b = inb(PS2_REG_STATUS);
    return *(ps2_status_t*)&b;
}

error_t ps2_init() {
    LOG_INFO("initializing ps2");

    // disable devices
    outb(PS2_REG_COMMAND, PS2_CMD_DISABLE_FIRST_PORT);
    outb(PS2_REG_COMMAND, PS2_CMD_DISABLE_SECOND_PORT);

    // flush the buffer
    inb(PS2_REG_DATA);

    // TODO add tests or whatever, should we even care?

    // enable devices
    outb(PS2_REG_COMMAND, PS2_CMD_ENABLE_FIRST_PORT);
    outb(PS2_REG_COMMAND, PS2_CMD_ENABLE_SECOND_PORT);

    return NO_ERROR;
}

bool ps2_poll() {
    return (bool) (!ps2_get_status().input_buffer && ps2_get_status().output_buffer);
}

error_t ps2_read(uint8_t* value) {
    error_t err = NO_ERROR;
    int timeout = 1000;
    while(ps2_get_status().input_buffer) {
        CHECK_ERROR(timeout--, ERROR_TIMEOUT);
    }
    *value = inb(PS2_REG_DATA);
cleanup:
    return NO_ERROR;
}

error_t ps2_write(int port, uint8_t value) {
    error_t err = NO_ERROR;

    int timeout = 1000;
    switch(port) {
        case PS2_FIRST:
            while(ps2_get_status().input_buffer) {
                CHECK_ERROR(timeout--, ERROR_TIMEOUT);
            }
            outb(PS2_REG_DATA, value);
            break;
        case PS2_SECOND:
            outb(PS2_REG_COMMAND, PS2_CMD_WRITE_SECOND_PORT);
            while(ps2_get_status().input_buffer) {
                CHECK_ERROR(timeout--, ERROR_TIMEOUT);
            }
            outb(PS2_REG_DATA, value);
            break;
        default:
            CHECK_FAIL_ERROR(ERROR_INVALID_PORT);
    }

cleanup:
    return err;
}
