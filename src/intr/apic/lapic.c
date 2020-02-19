#include <arch/msr.h>
#include <smp/pcpu.h>
#include <util/def.h>
#include <mm/vmm.h>
#include <util/trace.h>
#include <intr/intr.h>
#include <arch/paging.h>
#include <proc/process.h>
#include "lapic.h"
#include "apic_spec.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Local apic helper functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define LOCAL_APIC_BASE 0xFEE00000ull

static uint32_t lapic_read(size_t size) {
    return POKE32(PHYSICAL_TO_DIRECT(LOCAL_APIC_BASE) + size);
}

static void lapic_write(size_t size, uint32_t value) {
    POKE32(PHYSICAL_TO_DIRECT(LOCAL_APIC_BASE) + size) = value;
}

size_t get_apic_id() {
    return lapic_read(XAPIC_ID_OFFSET) >> 24u;
}

/*
 * What am I supposed to do here
 */
static err_t spurious_interrupt_handler(void* handler, interrupt_context_t* context) {
    send_apic_eoi();
    return NO_ERROR;
}

static interrupt_handler_t sv_handler = {
    .vector = 0xff,
    .name = "Spurious Interrupt",
    .callback = spurious_interrupt_handler,
};

static bool initialized_once = false;

void lapic_init() {
    LOCAL_APIC_LVT_LINT lint = {0};

    // only initialize once
    if (!initialized_once) {
        interrupt_register(&sv_handler);
        initialized_once = true;
    }

    // set the apic base of the cpu
    vmm_map(&kernel_process.vmm_handle, LOCAL_APIC_BASE, PHYSICAL_TO_DIRECT(LOCAL_APIC_BASE), PAGE_SIZE, PAGE_SUPERVISOR_READWRITE, DEFAULT_CACHE);
    TRACE("Local APIC init #%lu", get_apic_id());

    // setup svr
    LOCAL_APIC_SVR svr = { .raw = lapic_read(XAPIC_SPURIOUS_VECTOR_OFFSET) };
    svr.software_enable = 1;
    svr.spurious_vector = sv_handler.vector;
    lapic_write(XAPIC_SPURIOUS_VECTOR_OFFSET, svr.raw);

    // setup LINT0 as ExtInt
    lint.raw = lapic_read(XAPIC_LVT_LINT0_OFFSET);
    lint.delivery_mode = LOCAL_APIC_DELIVERY_MODE_EXTINT;
    lapic_write(XAPIC_LVT_LINT0_OFFSET, lint.raw);

    // setup LINT1 as NMI
    lint.raw = lapic_read(XAPIC_LVT_LINT1_OFFSET);
    lint.delivery_mode = LOCAL_APIC_DELIVERY_MODE_NMI;
    lapic_write(XAPIC_LVT_LINT1_OFFSET, lint.raw);
}

void send_fixed_ipi(uint32_t apic_id, uint8_t vector) {

}

void send_apic_eoi() {
    lapic_write(XAPIC_EOI_OFFSET, 0);
}
