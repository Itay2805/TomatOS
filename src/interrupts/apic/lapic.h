#ifndef TOMATKERNEL_LAPIC_H
#define TOMATKERNEL_LAPIC_H

#include <error.h>
#include <interrupts/interrupts.h>

// Lapic info
#define LAPIC_REG_ID            0x20
#define LAPIC_REG_VERSION       0x30

// Task/Arbitration/Processor Priority Register
#define LAPIC_REG_TPR           0x80
#define LAPIC_REG_APR           0x90
#define LAPIC_REG_PPR           0xA0

// End Of Interrupt
#define LAPIC_REG_EOI           0xB0

// Spurious Interrupt Vector Register
#define LAPIC_REG_SIVR          0xF0

// In-Service Register
#define LAPIC_REG_ISR0          0x100
#define LAPIC_REG_ISR1          0x110
#define LAPIC_REG_ISR2          0x120
#define LAPIC_REG_ISR3          0x130
#define LAPIC_REG_ISR4          0x140
#define LAPIC_REG_ISR5          0x150
#define LAPIC_REG_ISR6          0x160
#define LAPIC_REG_ISR7          0x170

// Trigger Mode Registe
#define LAPIC_REG_TMR0          0x180
#define LAPIC_REG_TMR1          0x190
#define LAPIC_REG_TMR2          0x1a0
#define LAPIC_REG_TMR3          0x1b0
#define LAPIC_REG_TMR4          0x1c0
#define LAPIC_REG_TMR5          0x1d0
#define LAPIC_REG_TMR6          0x1e0
#define LAPIC_REG_TMR7          0x1f0

// Interrupt Request Register
#define LAPIC_REG_IRR0          0x200
#define LAPIC_REG_IRR1          0x210
#define LAPIC_REG_IRR2          0x220
#define LAPIC_REG_IRR3          0x230
#define LAPIC_REG_IRR4          0x240
#define LAPIC_REG_IRR5          0x250
#define LAPIC_REG_IRR6          0x260
#define LAPIC_REG_IRR7          0x270

// Interrupt Command Register (64bit over two registers)
#define LAPIC_REG_ICR0          0x300
#define LAPIC_REG_ICR1          0x310

// Local Vector Table
#define LAPIC_REG_LVT_TIMER     0x320
#define LAPIC_REG_LVT_THERMAL   0x330
#define LAPIC_REG_LVT_MONITOR   0x340
#define LAPIC_REG_LVT_LINT0     0x350
#define LAPIC_REG_LVT_LINT1     0x360
#define LAPIC_REG_LVT_ERROR     0x370

// Timer
#define LAPIC_REG_TIMER_INITIAL_COUNT   0x380
#define LAPIC_REG_TIMER_CURRENT_COUNT   0x390
#define LAPIC_REG_TIMER_DEVIDER         0x3E0

#define LAPIC_DELIVERY_MODE_FIXED           0
#define LAPIC_DELIVERY_MODE_LOWEST_PRIORITY 1
#define LAPIC_DELIVERY_MODE_SMI             2
#define LAPIC_DELIVERY_MODE_NMI             4
#define LAPIC_DELIVERY_MODE_INIT            5
#define LAPIC_DELIVERY_MODE_STARTUP         6
#define LAPIC_DELIVERY_MODE_EXTINT          7

#define LAPIC_TIMER_DIVIDER_2    0b0000
#define LAPIC_TIMER_DIVIDER_4    0b0001
#define LAPIC_TIMER_DIVIDER_8    0b0010
#define LAPIC_TIMER_DIVIDER_16   0b0011
#define LAPIC_TIMER_DIVIDER_32   0b1000
#define LAPIC_TIMER_DIVIDER_64   0b1001
#define LAPIC_TIMER_DIVIDER_128  0b1010
#define LAPIC_TIMER_DIVIDER_1    0b1011

#ifndef LAPIC_SPURIOUS_VECTOR
    #define LAPIC_SPURIOUS_VECTOR   0xFF
#endif

#define LAPIC_TIMER_MODE_ONE_SHOT   0
#define LAPIC_TIMER_MODE_PERIODIC   1

typedef union lapic_lvt_timer {
    struct {
        uint32_t vector : 8;
        uint32_t _reserved1 : 4;
        uint32_t delivery_status : 1;
        uint32_t _reserved2 : 3;
        uint32_t mask : 1;
        uint32_t timer_mode : 1;
        uint32_t _reserved3 : 14;
    } __attribute__((packed));
    uint32_t raw;
} lapic_lvt_timer_t;

typedef union lapic_lvt_lint {
    struct {
        uint32_t vector : 8;
        uint32_t delivery_mode : 3;
        uint32_t _reserved1 : 1;
        uint32_t delivery_status : 1;
        uint32_t input_pin_polarity : 1;
        uint32_t remote_irr : 1;
        uint32_t trigger_mode : 1;
        uint32_t mask : 1;
        uint32_t _reserved2 : 15;
    } __attribute__((packed));
    uint32_t raw;
} lapic_lvt_lint_t;


typedef union lapic_svr {
    struct {
        uint32_t spurious_vector : 8;
        uint32_t software_enable : 1;
        uint32_t focus_processor_checking : 1;
        uint32_t _reserved1 : 2;
        uint32_t eoi_broadcast_suppression : 1;
        uint32_t _reserved2 : 19;
    } __attribute__((packed));
    uint32_t raw;
} lapic_svr_t;

typedef union lapic_icr {
    struct {
        uint64_t vector : 8;
        uint64_t delivery_mode : 3;
        uint64_t destination_mode : 1;
        uint64_t _reserved0 : 1;
        uint64_t level : 1;
        uint64_t trigger_mode : 1;
        uint64_t _reserved2 : 2;
        uint64_t destination_shothand : 2;
        uint64_t _reserved3 : 36;
        uint64_t destination : 8;
    } __attribute__((packed));
    struct {
        uint32_t raw_low;
        uint32_t raw_high;
    } __attribute__((packed));
    uint64_t raw;
} lapic_icr_t;

typedef union lapic_msi_address {
    struct {
        uint32_t _reserved0 : 2;
        uint32_t destination_mode : 1;
        uint32_t redirection_hint : 1;
        uint32_t _reserved1 : 8;
        uint32_t destination_id : 8;
        uint32_t base_address : 12; // 0x0FEE
    } __attribute__((packed));
    uint32_t raw;
} lapic_msi_address_t;

typedef union lapic_msi_data {
    struct {
        uint8_t vector;

        uint8_t delivery_mode : 3;
        uint8_t _reserved0 : 3;
        uint8_t level : 1;
        uint8_t trigger_mode : 1;

        uint16_t _reserved1;
    } __attribute__((packed));
    uint32_t raw;
} lapic_msi_data_t;


/**
 * Initialize the Local APIC
 *
 * This will set the NMIs and Spurious interrupts
 * @return
 */
error_t lapic_init();

/**
 * Write into a lapic register
 *
 * @param reg       [IN] The register
 * @param value     [IN] The value
 */
void lapic_write(uint32_t reg, uint32_t value);

/**
 * Read from a lapic register
 *
 * @param reg       [IN] The register
 */
uint32_t lapic_read(uint32_t reg);

/**
 * Get the ID of the Local APIC
 */
uint8_t lapic_get_id();

/**
 * Enable the Local APIC
 *
 * Will set the enable bit and the spurious interrupt vector
 */
error_t lapic_enable();

/**
 * Setup the lapic timer to fire after the given amount of time and trigger
 * the given vector
 */
error_t lapic_set_timer(uint32_t millis, uint8_t vector);

/**
 * Checks if the lapic timer has fired
 * @return
 */
bool lapic_timer_fired();

/**
 * Send an EOI to the Local APIC controller
 */
error_t lapic_send_eoi();

#endif //TOMATKERNEL_LAPIC_H
