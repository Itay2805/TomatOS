#include "pit.h"
#include "portio.h"

////////////////////////////////////////////////////////////////////////////
// The IO ports
////////////////////////////////////////////////////////////////////////////

#define PIT_DATA_CHANNEL_0 0x40
#define PIT_DATA_CHANNEL_1 0x41
#define PIT_DATA_CHANNEL_2 0x42
#define PIT_COMMAND 0x43

////////////////////////////////////////////////////////////////////////////
// Some constants for the command
////////////////////////////////////////////////////////////////////////////

#define MODE_BIN (0 << 0)
#define MODE_BCD (1 << 0)

#define OP_TRIGGER_ON_TERMINAL (uint8_t)(0u << 1u)
#define OP_ONE_SHOT (uint8_t)(1u << 1u)
#define OP_RATE_GEN (uint8_t)(2u << 1u)
#define OP_SQUARE_WAVE (uint8_t)(3u << 1u)
#define OP_SOFT_TRIGGERED_PROBE (uint8_t)(4u << 1u)
#define OP_HARD_TRIGGERED_PROBE (uint8_t)(5u << 1u)

#define CHANNEL_IRQ (uint8_t)(0u << 6u)
// #define CHANNEL_1 (uint8_t)(1u << 6u)
#define CHANNEL_SPEAKER (uint8_t)(2u << 6u)

#define ACCESS_LATCH_COUNT (uint8_t)(0u << 4u)
#define ACCESS_LOW_BYTE (uint8_t)(1u << 4u)
#define ACCESS_HIGH_BYTE (uint8_t)(2u << 4u)
#define ACCESS_SHORT (uint8_t)(3u << 4u)

////////////////////////////////////////////////////////////////////////////
// The implementation
////////////////////////////////////////////////////////////////////////////

/**
 * The current interval
 */
static uint64_t interval = 0;

/**
 * convert the interval (in milliseconds) to the divisor
 * for the pit
 */
inline static uint64_t interval_to_divisor(uint64_t interval) {
    return (715909 * interval) / 600;
}

void pit_set_interval(uint64_t i) {
    interval = i;

    outb(PIT_COMMAND, OP_RATE_GEN | CHANNEL_IRQ | ACCESS_SHORT);

    // TODO: Check the divisor is not too big
    uint16_t divisor = (uint16_t) interval_to_divisor(interval);
    outb(PIT_DATA_CHANNEL_0, (uint8_t) (divisor & 0xFF));
    outb(PIT_DATA_CHANNEL_0, (uint8_t) ((divisor >> 16u) & 0xFF));
}

uint64_t pit_get_interval() {
    return interval;
}
