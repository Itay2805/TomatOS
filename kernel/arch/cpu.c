#include <util/trace.h>
#include <mem/vmm.h>
#include <util/except.h>
#include "intrin.h"
#include "cpu.h"
#include "io.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CPU Local variables
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// declare globals
uintptr_t g_cpu_locals[256] = {0};
size_t g_cpu_count = 0;
size_t CPU_LOCAL g_cpu_id;
size_t CPU_LOCAL g_lapic_id;

void init_cpu_locals(size_t cpuid, size_t lapic_id) {
    // set the correct thing
    __wrmsr(MSR_IA32_GS_BASE, (uintptr_t)g_cpu_locals[cpuid]);

    // set some globals
    g_cpu_id = cpuid;
    g_lapic_id = lapic_id;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Other cpu related operations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void cpu_pause() {
    asm volatile ("pause");
}

void cpu_sleep() {
    __hlt();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Memory fences and barriers
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void memory_barrier() {
    asm volatile ("" ::: "memory");
}

void store_fence() {
    asm volatile ("sfence" ::: "memory");
}

void load_fence() {
    asm volatile ("lfence" ::: "memory");
}

void memory_fence() {
    asm volatile ("mfence" ::: "memory");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Interrupt management
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void disable_interrupts() {
    asm volatile("cli" ::: "memory");
}

void enable_interrupts() {
    asm volatile("sti" ::: "memory");
}

bool are_interrupts_enabled() {
    IA32_RFLAGS flags;
    asm volatile (
        "pushfq\n"
        "pop %0"
        : "=r" (flags.raw)
    );
    return flags.IF == 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// IPIs
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
// Helper structures
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

#define LAPIC_DELIVERY_MODE_FIXED           0
#define LAPIC_DELIVERY_MODE_LOWEST_PRIORITY 1
#define LAPIC_DELIVERY_MODE_SMI             2
#define LAPIC_DELIVERY_MODE_NMI             4
#define LAPIC_DELIVERY_MODE_INIT            5
#define LAPIC_DELIVERY_MODE_STARTUP         6
#define LAPIC_DELIVERY_MODE_EXTINT          7

#define LAPIC_DESTINATION_SHORTHAND_NO_SHORTHAND       0
#define LAPIC_DESTINATION_SHORTHAND_SELF               1
#define LAPIC_DESTINATION_SHORTHAND_ALL_INCLUDING_SELF 2
#define LAPIC_DESTINATION_SHORTHAND_ALL_EXCLUDING_SELF 3

typedef union lapic_icr_low {
    struct {
        uint32_t vector : 8;
        uint32_t delivery_mode : 3;
        uint32_t destination_mode : 1;
        uint32_t delivery_status : 1;
        uint32_t _reserved0 : 1;
        uint32_t level : 1;
        uint32_t trigger_mode : 1;
        uint32_t _reserved1 : 2;
        uint32_t destination_shorthand : 2;
        uint32_t _reserved2 : 12;
    };
    uint32_t raw;
} lapic_icr_low_t;

typedef union lapic_svr {
    struct {
        uint32_t spurious_vector : 8;
        uint32_t software_enable : 1;
        uint32_t focus_processor_checking : 1;
        uint32_t _reserved0 : 2;
        uint32_t eoi_broadcast_suppression : 1;
        uint32_t _reserved1 : 19;
    };
    uint32_t raw;
} lapic_svr_t;

/**
 * This is the base of the apic
 */
static uint64_t m_apic_base = 0;

/**
 * Read a lapic register
 */
static uint32_t read_lapic_reg(size_t mmio_offset) {
    return mmio_read_32(m_apic_base + mmio_offset);
}

/**
 * Write to a lapic register
 */
static void write_lapic_reg(size_t mmio_offset, uint32_t value) {
    return mmio_write_32(m_apic_base + mmio_offset, value);
}

void init_lapic() {
    if (m_apic_base == 0) {
        m_apic_base = __rdmsr(MSR_IA32_APIC_BASE) & ~0xfff;
        ASSERT(!IS_ERROR(vmm_map((uintptr_t)PHYS_TO_DIRECT(m_apic_base), m_apic_base,
                                     SIZE_TO_PAGES(0x100),
                                     MAP_READ | MAP_WRITE)));
        m_apic_base = (uintptr_t)PHYS_TO_DIRECT(m_apic_base);
    }

    // setup the spurious vector
    lapic_svr_t svr = {
        .software_enable = 1,
        .spurious_vector = 0xFF,
    };
    write_lapic_reg(XAPIC_SPURIOUS_VECTOR_OFFSET, svr.raw);
}

/**
 * Send an ipi given the icr and id
 */
static void lapic_send_ipi(uint32_t icr_low, uint32_t apic_id) {
    bool state = are_interrupts_enabled();
    disable_interrupts();

    // write the regs in the right order
    write_lapic_reg(XAPIC_ICR_HIGH_OFFSET, apic_id << 24);
    write_lapic_reg(XAPIC_ICR_LOW_OFFSET, icr_low);

    // wait for delivery
    lapic_icr_low_t icr = { 0 };
    do {
        icr.raw = read_lapic_reg(XAPIC_ICR_LOW_OFFSET);
    } while (icr.delivery_status != 0);

    if (state) {
        enable_interrupts();
    }
}

void lapic_send_fixed_ipi_all_excluding_self(uint8_t vector) {
    lapic_icr_low_t icrlow = {
        .vector = vector,
        .delivery_mode = LAPIC_DELIVERY_MODE_FIXED,
        .level = 1,
        .destination_shorthand = LAPIC_DESTINATION_SHORTHAND_ALL_EXCLUDING_SELF,
    };
    lapic_send_ipi(icrlow.raw, 0);
}
