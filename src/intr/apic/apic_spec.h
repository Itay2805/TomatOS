#ifndef __INTR_APIC_APIC_SPEC_H__
#define __INTR_APIC_APIC_SPEC_H__

#include <stdint.h>
#include <assert.h>

///
/// I/O APIC Register Offsets
///
#define IOAPIC_INDEX_OFFSET  0x00
#define IOAPIC_DATA_OFFSET   0x10

///
/// I/O APIC Indirect Register Indexes
///
#define IO_APIC_IDENTIFICATION_REGISTER_INDEX  0x00
#define IO_APIC_VERSION_REGISTER_INDEX         0x01
#define IO_APIC_REDIRECTION_TABLE_ENTRY_INDEX  0x10

typedef union {
    struct {
        uint32_t  _reserved0:24;
        uint32_t  identification:4;
        uint32_t  _reserved1:4;
    };
    uint32_t raw;
} IO_APIC_IDENTIFICATION_REGISTER;
static_assert(sizeof(IO_APIC_IDENTIFICATION_REGISTER) == sizeof(uint32_t));

typedef union {
    struct {
        uint32_t  version:8;
        uint32_t  _reserved0:8;
        uint32_t  maximum_redirection_entry:8;
        uint32_t  _reserved1:8;
    };
    uint32_t raw;
} IO_APIC_VERSION_REGISTER;
static_assert(sizeof(IO_APIC_VERSION_REGISTER) == sizeof(uint32_t));

typedef union {
    struct {
        uint32_t  vector:8;
        uint32_t  delivery_mode:3;
        uint32_t  destination_mode:1;
        uint32_t  delivery_status:1;
        uint32_t  polarity:1;
        uint32_t  remote_irr:1;
        uint32_t  trigger_mode:1;
        uint32_t  mask:1;
        uint32_t  _reserved0:15;
        uint32_t  _reserved1:24;
        uint32_t  destination_id:8;
    };
    struct {
        uint32_t low;
        uint32_t high;
    } raw_split;
    uint64_t raw;
} IO_APIC_REDIRECTION_TABLE_ENTRY;
static_assert(sizeof(IO_APIC_REDIRECTION_TABLE_ENTRY) == sizeof(uint64_t));

//
// Definition for Local APIC registers and related values
//
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

//
// Local APIC Version Register.
//
typedef union {
    struct {
        uint32_t  version:8;                  ///< The version numbers of the local APIC.
        uint32_t  _reserved0:8;                ///< Reserved.
        uint32_t  max_lvt_entry:8;              ///< Number of LVT entries minus 1.
        uint32_t  eoi_broadcast_suppression:1;  ///< 1 if EOI-broadcast suppression supported.
        uint32_t  _reserved1:7;                ///< Reserved.
    };
    uint32_t raw;
} LOCAL_APIC_VERSION;
static_assert(sizeof(LOCAL_APIC_VERSION) == sizeof(uint32_t));

//
// Low half of Interrupt Command Register (ICR).
//
typedef union {
    struct {
        uint32_t  vector:8;                ///< The vector number of the interrupt being sent.
        uint32_t  delivery_mode:3;          ///< Specifies the type of IPI to be sent.
        uint32_t  destination_mode:1;       ///< 0: physical destination mode, 1: logical destination mode.
        uint32_t  delivery_status:1;        ///< Indicates the IPI delivery status. This field is reserved in x2APIC mode.
        uint32_t  _reserved0:1;             ///< Reserved.
        uint32_t  level:1;                 ///< 0 for the INIT level de-assert delivery mode. Otherwise 1.
        uint32_t  trigger_mode:1;           ///< 0: edge, 1: level when using the INIT level de-assert delivery mode.
        uint32_t  _reserved1:2;             ///< Reserved.
        uint32_t  destination_shorthand:2;  ///< A shorthand notation to specify the destination of the interrupt.
        uint32_t  _reserved2:12;            ///< Reserved.
    };
    uint32_t raw;
} LOCAL_APIC_ICR_LOW;
static_assert(sizeof(LOCAL_APIC_ICR_LOW) == sizeof(uint32_t));

//
// High half of Interrupt Command Register (ICR)
//
typedef union {
    struct {
        uint32_t  _reserved0:24;   ///< Reserved.
        uint32_t  destination:8;  ///< Specifies the target processor or processors in xAPIC mode.
    };
    uint32_t raw;         ///< Destination field expanded to 32-bit in x2APIC mode.
} LOCAL_APIC_ICR_HIGH;
static_assert(sizeof(LOCAL_APIC_ICR_HIGH) == sizeof(uint32_t));

//
// Spurious-Interrupt Vector Register (SVR)
//
typedef union {
    struct {
        uint32_t  spurious_vector:8;           ///< Spurious Vector.
        uint32_t  software_enable:1;           ///< APIC Software Enable/Disable.
        uint32_t  focus_processor_checking:1;   ///< Focus Processor Checking.
        uint32_t  _reserved0:2;                ///< Reserved.
        uint32_t  eoi_broadcast_suppression:1;  ///< EOI-Broadcast Suppression.
        uint32_t  _reserved1:19;               ///< Reserved.
    };
    uint32_t raw;
} LOCAL_APIC_SVR;
static_assert(sizeof(LOCAL_APIC_SVR) == sizeof(uint32_t));

//
// Divide Configuration Register (DCR)
//
typedef union {
    struct {
        uint32_t  divide_value1:2;  ///< Low 2 bits of the divide value.
        uint32_t  _reserved0:1;     ///< Always 0.
        uint32_t  divide_value2:1;  ///< Highest 1 bit of the divide value.
        uint32_t  _reserved1:28;    ///< Reserved.
    };
    uint32_t raw;
} LOCAL_APIC_DCR;
static_assert(sizeof(LOCAL_APIC_DCR) == sizeof(uint32_t));

//
// LVT Timer Register
//
typedef union {
    struct {
        uint32_t  vector:8;          ///< The vector number of the interrupt being sent.
        uint32_t  _reserved0:4;       ///< Reserved.
        uint32_t  delivery_status:1;  ///< 0: Idle, 1: send pending.
        uint32_t  _reserved1:3;       ///< Reserved.
        uint32_t  mask:1;            ///< 0: Not masked, 1: Masked.
        uint32_t  timer_mode:1;       ///< 0: One-shot, 1: Periodic.
        uint32_t  _reserved2:14;      ///< Reserved.
    };
    uint32_t raw;
} LOCAL_APIC_LVT_TIMER;
static_assert(sizeof(LOCAL_APIC_LVT_TIMER) == sizeof(uint32_t));

//
// LVT LINT0/LINT1 Register
//
typedef union {
    struct {
        uint32_t  vector:8;            ///< The vector number of the interrupt being sent.
        uint32_t  delivery_mode:3;      ///< Specifies the type of interrupt to be sent.
        uint32_t  _reserved0:1;         ///< Reserved.
        uint32_t  delivery_dtatus:1;    ///< 0: Idle, 1: send pending.
        uint32_t  input_pin_polarity:1;  ///< Interrupt Input Pin Polarity.
        uint32_t  remote_irr:1;         ///< RO. Set when the local APIC accepts the interrupt and reset when an EOI is received.
        uint32_t  trigger_mode:1;       ///< 0:edge, 1:level.
        uint32_t  mask:1;              ///< 0: Not masked, 1: Masked.
        uint32_t  _reserved1:15;        ///< Reserved.
    };
    uint32_t raw;
} LOCAL_APIC_LVT_LINT;
static_assert(sizeof(LOCAL_APIC_LVT_LINT) == sizeof(uint32_t));

#endif //__INTR_APIC_APIC_SPEC_H__
