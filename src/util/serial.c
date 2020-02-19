#include <arch/intrin.h>

#include "serial.h"
#include "except.h"

//---------------------------------------------
// UART Register Offsets
//---------------------------------------------
#define BAUD_LOW_OFFSET         0x00u
#define BAUD_HIGH_OFFSET        0x01u
#define IER_OFFSET              0x01u
#define LCR_SHADOW_OFFSET       0x01u
#define FCR_SHADOW_OFFSET       0x02u
#define IR_CONTROL_OFFSET       0x02u
#define FCR_OFFSET              0x02u
#define EIR_OFFSET              0x02u
#define BSR_OFFSET              0x03u
#define LCR_OFFSET              0x03u
#define MCR_OFFSET              0x04u
#define LSR_OFFSET              0x05u
#define MSR_OFFSET              0x06u

//---------------------------------------------
// UART Register Bit Defines
//---------------------------------------------
#define LSR_TXRDY               0x20u
#define LSR_RXDA                0x01u
#define DLAB                    0x01u
#define MCR_DTRC                0x01u
#define MCR_RTS                 0x02u
#define MSR_CTS                 0x10u
#define MSR_DSR                 0x20u
#define MSR_RI                  0x40u
#define MSR_DCD                 0x80u

//---------------------------------------------
// Default configurations
//---------------------------------------------
#define UART_BASE 0x3F8
#define UART_BAUD 115200
#define UART_DATA 8
#define UART_STOP 1
#define UART_PARITY 0
#define UART_BREAK_SET 0

void serial_init() {
    // set the communication format
    uint8_t data = UART_DATA - 5;
    uint8_t output_data = ((DLAB << 7u) | (UART_BREAK_SET << 6u) | (UART_PARITY << 3u) | (UART_STOP << 2u) | data);
    __outbyte(UART_BASE + LCR_OFFSET, output_data);

    // configure baud rate
    size_t divisor = 115200 / UART_BAUD;
    __outbyte(UART_BASE + BAUD_HIGH_OFFSET, divisor >> 8);
    __outbyte(UART_BASE + BAUD_HIGH_OFFSET, divisor & 0xff);

    // switch to bank 0
    output_data = ((UART_BREAK_SET << 6u) | (UART_PARITY << 3u) | (UART_STOP << 2u) | data);
    __outbyte(UART_BASE + LCR_OFFSET, output_data);
}

void serial_write(const char* buffer, size_t size) {
    ASSERT(buffer != NULL);

    while (size--) {
        // wait for the tx to be ready
        uint8_t data;
        do {
            data = __inbyte(UART_BASE + LSR_OFFSET);
        } while((data & LSR_TXRDY) == 0);

        // write it
        __outbyte(UART_BASE, *buffer++);
    }
}

void serial_read(char* buffer, size_t bytes) {
    while (bytes--) {
        // wait for the rx to be ready
        uint8_t data;
        do {
            data = __inbyte(UART_BASE + LSR_OFFSET);
        } while((data & LSR_RXDA) == 0);

        // read
        *buffer++ = __inbyte(UART_BASE);
    }
}

bool serial_poll() {
    return __inbyte(UART_BASE + LSR_OFFSET) & LSR_RXDA;
}
