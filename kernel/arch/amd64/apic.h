#ifndef __TOMATOS_KERNEL_ARCH_AMD64_APIC_H__
#define __TOMATOS_KERNEL_ARCH_AMD64_APIC_H__

#include <acpi/tables/madt.h>
#include <util/except.h>
#include <util/defs.h>
#include <stdint.h>


#define XAPIC_ID_OFFSET                         0x20
#define XAPIC_VERSION_OFFSET                    0x30
#define XAPIC_EOI_OFFSET                        0x0b0
#define XAPIC_ICR_DFR_OFFSET                    0x0e0
#define XAPIC_SPURIOUS_VECTOR_OFFSET            0x0f0
#define XAPIC_ICR_LOW_OFFSET                    0x300
#define XAPIC_ICR_HIGH_OFFSET                   0x310
#define XAPIC_LVT_TIMER_OFFSET                  0x320
#define XAPIC_LVT_LINT0_OFFSET                  0x350
#define XAPIC_LVT_LINT1_OFFSET                  0x360
#define XAPIC_TIMER_INIT_COUNT_OFFSET           0x380
#define XAPIC_TIMER_CURRENT_COUNT_OFFSET        0x390
#define XAPIC_TIMER_DIVIDE_CONFIGURATION_OFFSET 0x3E0

#define X2APIC_MSR_BASE_ADDRESS                 0x800
#define X2APIC_MSR_ICR_ADDRESS                  0x830

#define LOCAL_APIC_DELIVERY_MODE_FIXED           0
#define LOCAL_APIC_DELIVERY_MODE_LOWEST_PRIORITY 1
#define LOCAL_APIC_DELIVERY_MODE_SMI             2
#define LOCAL_APIC_DELIVERY_MODE_NMI             4
#define LOCAL_APIC_DELIVERY_MODE_INIT            5
#define LOCAL_APIC_DELIVERY_MODE_STARTUP         6
#define LOCAL_APIC_DELIVERY_MODE_EXTINT          7

#define LOCAL_APIC_DESTINATION_SHORTHAND_NO_SHORTHAND       0
#define LOCAL_APIC_DESTINATION_SHORTHAND_SELF               1
#define LOCAL_APIC_DESTINATION_SHORTHAND_ALL_INCLUDING_SELF 2
#define LOCAL_APIC_DESTINATION_SHORTHAND_ALL_EXCLUDING_SELF 3

typedef union lapic_version {
    struct {
        uint32_t version : 8;                   ///< The version numbers of the local APIC.
        uint32_t _reserved : 8;                 ///< Reserved.
        uint32_t max_lvt_entry : 8;             ///< Number of LVT entries minus 1.
        uint32_t eoi_broadcast_supression : 1;  ///< 1 if EOI-broadcast suppression supported.
        uint32_t _reserved1 : 7;                ///< Reserved.
    };
    uint32_t raw;
} PACKED lapic_version_t;

typedef union lapic_icr_low {
    struct {
        uint32_t vector : 8;                    ///< The vector number of the interrupt being sent.
        uint32_t delivery_mode : 3;             ///< Specifies the type of IPI to be sent.
        uint32_t destination_mode : 1;          ///< 0: physical destination mode, 1: logical destination mode.
        uint32_t delivery_status : 1;           ///< Indicates the IPI delivery status. This field is reserved in x2APIC mode.
        uint32_t _reserved0 : 1;                ///< Reserved.
        uint32_t level : 1;                     ///< 0 for the INIT level de-assert delivery mode. Otherwise 1.
        uint32_t trigger_mode : 1;              ///< 0: edge, 1: level when using the INIT level de-assert delivery mode.
        uint32_t _reserved1 : 2;                ///< Reserved.
        uint32_t destination_shorthand : 2;     ///< A shorthand notation to specify the destination of the interrupt.
        uint32_t _reserved2 : 12;               ///< Reserved.
    };
    uint32_t raw;
} PACKED lapic_icr_low_t;

typedef union lapic_icr_high {
    struct {
        uint32_t _reserved0 : 24;               ///< Reserved.
        uint32_t destination : 8;               ///< Specifies the target processor or processors in xAPIC mode.
    };
    uint32_t raw;                               ///< Destination field expanded to 32-bit in x2APIC mode.
} PACKED lapic_icr_high_t;

typedef union lapic_svr {
    struct {
        uint32_t spurious_vector : 8;           ///< Spurious Vector.
        uint32_t software_enable : 1;           ///< APIC Software Enable/Disable.
        uint32_t focus_processor_checking : 1;  ///< Focus Processor Checking.
        uint32_t _reserved : 2;                 ///< Reserved.
        uint32_t eoi_broadcast_supression : 1;  ///< EOI-Broadcast Suppression.
        uint32_t _Reserved1 : 19;               ///< Reserved.
    };
    uint32_t raw;
} PACKED lapic_svr_t;

typedef union lapic_lvt_lint {
    struct {
        uint32_t vector : 8;                    ///< The vector number of the interrupt being sent.
        uint32_t delivery_mode : 3;             ///< Specifies the type of interrupt to be sent.
        uint32_t _reserved0 : 1;                ///< Reserved.
        uint32_t delivery_status : 1;           ///< 0: Idle, 1: send pending.
        uint32_t input_pin_polarity : 1;        ///< Interrupt Input Pin Polarity.
        uint32_t remote_irr : 1;                ///< RO. Set when the local APIC accepts the interrupt and reset when an EOI is received.
        uint32_t trigger_mode : 1;              ///< 0:edge, 1:level.
        uint32_t mask : 1;                      ///< 0: Not masked, 1: Masked.
        uint32_t _reserved1 : 15;               ///< Reserved.
    };
    uint32_t raw;
} PACKED lapic_lvt_lint_t;

typedef union lapic_lvt_timer {
        struct {
            uint32_t vector : 8;                ///< The vector number of the interrupt being sent.
            uint32_t _reserved0 : 4;            ///< Reserved.
            uint32_t delivery_status : 1;       ///< 0: Idle, 1: send pending.
            uint32_t _reserved1 : 3;            ///< Reserved.
            uint32_t mask : 1;                  ///< 0: Not masked, 1: Masked.
            uint32_t timer_mode : 1;            ///< 0: One-shot, 1: Periodic.
            uint32_t _reserved : 14;            ///< Reserved.
        };
        uint32_t raw;
} lapic_lvt_timer_t;


#define FOR_EACH_IN_MADT() \
    for (madt_entry_t* entry = (void*)(g_madt + 1); (uint64_t) entry < (uint64_t) g_madt + g_madt->header.length; entry = (madt_entry_t*)((uint64_t)entry + entry->length))

extern acpi_madt_t* g_madt;

/**
 * Initialize the local apic
 */
err_t init_lapic();

/**
 * Send an eoi to the current lapic
 */
void send_lapic_eoi();

/**
 * Starts all the cores not including self
 */
err_t startup_all_cores();

/**
 * Get the ID of the current lapic
 */
uint32_t get_lapic_id();

/**
 * Will set the lapic timer for the next interrupt
 */
err_t setup_lapic_timer(uint64_t ticks);

#endif //__TOMATOS_KERNEL_ARCH_AMD64_APIC_H__
