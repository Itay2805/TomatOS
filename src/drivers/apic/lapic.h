#ifndef TOMATKERNEL_LAPIC_H
#define TOMATKERNEL_LAPIC_H

#include <error.h>

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

#define LAPIC_LVT_FIXED     0b000
#define LAPIC_LVT_SMI       0b010
#define LAPIC_LVT_NMI       0b100
#define LAPIC_LVT_ExtINT    0b111
#define LAPIC_LVT_INIT      0b101

#define LAPIC_TIMER_DIVIDER_2    0b000
#define LAPIC_TIMER_DIVIDER_4    0b001
#define LAPIC_TIMER_DIVIDER_8    0b010
#define LAPIC_TIMER_DIVIDER_16   0b011
#define LAPIC_TIMER_DIVIDER_32   0b100
#define LAPIC_TIMER_DIVIDER_64   0b101
#define LAPIC_TIMER_DIVIDER_128  0b110
#define LAPIC_TIMER_DIVIDER_1    0b111

#ifndef LAPIC_SPURIOUS_VECTOR
    #define LAPIC_SPURIOUS_VECTOR   0xFF
#endif

typedef struct lapic_lvt {
    // all
    uint32_t vector : 8;

    // reserved in timer
    uint32_t delievery_mode : 3;

    // all
    uint32_t reserved : 1;
    uint32_t send_pending : 1;

    // reserved in timer
    uint32_t polarity : 1;
    uint32_t remote_irr : 1;
    uint32_t level_triggered : 1;

    // all
    uint32_t masked : 1;

    // only for timer
    uint32_t periodic : 1;
} __attribute__((packed)) lapic_lvt_t;

typedef struct lapic_svr {
    uint32_t vector : 8;
    uint32_t enabled : 1;
    uint32_t focus : 1;
} __attribute__((packed)) lapic_svr_t;

// TODO: ICR, only needed for IPI

/**
 * This is the ticks per seconds of the timer
 */
extern uint32_t lapic_timer_ticks_per_second;

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
 * Send an EOI to the Local APIC controller
 */
error_t lapic_send_eoi();

#endif //TOMATKERNEL_LAPIC_H
