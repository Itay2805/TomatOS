#ifndef TOMATKERNEL_HPET_H
#define TOMATKERNEL_HPET_H

#include <stdint.h>
#include <error.h>
#include <interrupts/interrupts.h>
#include <interrupts/apic/lapic.h>

// relative to hpet addr
#define HPET_REG_GENERAL_CAPS               0x00
#define HPET_REG_GENERAL_CONFIG             0x10
#define HPET_REG_GENERAL_INTERRUPT_STATUS   0x20
#define HPET_REG_MAIN_COUNTER_VALUE         0xF0

// relative to timer base
#define HPET_REG_TIMER_CONFIGURATION        0x00
#define HPET_REG_TIMER_COMPARATOR_VALUE     0x08
#define HPET_REG_TIMER_MSI_CONFIG           0x10

#define HPET_TIMER_BASE_OFFSET(n)           (0x100u + n * 0x20u)

#define FEMTO_TO_MILLIS(n) (n / 1000000000000)
#define MILLIS_TO_FEMTO(n) (n * 1000000000000)

typedef union hpet_reg_general_cap {
    struct {
        uint8_t rev_id;

        uint8_t timer_count : 5;
        uint8_t counter_64bit : 1;
        uint8_t _reserved : 1;
        uint8_t legacy_replacement_capable : 1;

        uint16_t vendor_id;

        uint32_t femto_per_Tick;
    }  __attribute__((packed));
    uint64_t raw;
} hpet_reg_general_cap_t;

typedef union hpet_reg_general_config {
    struct {
        uint8_t enable : 1;
        uint8_t legacy_replacement : 1;
        uint8_t _reserved0 : 6;

        uint8_t _reserved1;
        uint16_t _reserved2;
        uint32_t _reserved3;
    } __attribute__((packed));
    uint64_t raw;
} hpet_reg_general_config_t;

#define HPET_TIMER_INTERRUPT_TYPE_EDGE_TRIGGER  0
#define HPET_TIMER_INTERRUPT_TYPE_LEVEL_TRIGGER 1

#define HPET_TIMER_TYPE_PERIODIC 0
#define HPET_TIMER_TYPE_ONE_TIME 1

typedef union hpet_reg_timer_config {
    struct {
        uint8_t _reserved0 : 1;
        uint8_t interrupt_type : 1;
        uint8_t interrupt_enable : 1;
        uint8_t type : 1;
        uint8_t supports_periodic : 1;
        uint8_t timer_64bit : 1;
        uint8_t VAL_SET_CNF : 1;
        uint8_t _reserved1 : 1;

        uint8_t timer_32bit_mode : 1;
        uint8_t INT_ROUTE_CNF : 5;
        uint8_t msi_enable : 1;
        uint8_t supports_msi : 1;

        uint16_t _reserved2;

        uint32_t INT_ROUTE_CAP;
    } __attribute__((packed));
    uint64_t raw;
} hpet_reg_timer_config_t;

typedef union hpet_reg_timer_fsb_interrupt_route {
    struct {
        lapic_msi_address_t address;
        lapic_msi_data_t data;
    } __attribute__((packed));
    uint64_t raw;
} hpet_reg_timer_msi_config_t;

typedef struct hpet_timer {
    size_t id;
    uint8_t vector;
} hpet_timer_t;

typedef struct hpet {
    volatile uint64_t* mmio;
    uint64_t femto_per_tick;
    hpet_timer_t* timers;
} hpet_t;

/**
 * Initialize the HPET
 */
error_t hpet_init();

/**
 * Will stall the cpu for the given amount of time
 *
 * @param millis    [IN] Time to stall for
 */
error_t hpet_stall(int64_t millis);

/**
 * Find and initialize all HPETs
 *
 * This will search the ACPI namespace for more HPETs to use
 *
 * @remark
 * But run after lai has been initialized
 */
error_t hpet_find_all();

typedef error_t (*hpet_timeout_handler_f)(registers_t* regs, void* user);

/**
 * Set a timeout timer
 */
error_t hpet_set_timeout(hpet_timeout_handler_f handler, void* user, uint64_t millis);

/**
 * Will get the millis since the hpet started
 */
uint64_t hpet_get_millis();

#endif //TOMATKERNEL_HPET_H
