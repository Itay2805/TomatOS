#include <lai/host.h>
#include <mem/vmm.h>
#include <sync/critical.h>
#include <arch/timing.h>
#include <util/string.h>
#include <proc/process.h>
#include <mem/mm.h>
#include "apic.h"
#include "intrin.h"
#include "idt.h"

#define SMP_FLAG                (0x510 + DIRECT_MAPPING_BASE)
#define SMP_KERNEL_ENTRY        (0x520 + DIRECT_MAPPING_BASE)
#define SMP_KERNEL_PAGE_TABLE   (0x540 + DIRECT_MAPPING_BASE)
#define SMP_STACK_POINTER       (0x550 + DIRECT_MAPPING_BASE)
#define SMP_KERNEL_GDT          (0x580 + DIRECT_MAPPING_BASE)
#define SMP_KERNEL_IDT          (0x590 + DIRECT_MAPPING_BASE)

/**
 * The mmio address used to talk with the lapic
 */
static uint8_t* g_lapic_base = NULL;

acpi_madt_t* g_madt = NULL;

static void lapic_write(size_t reg, uint32_t value) {
    *(volatile uint32_t*)(g_lapic_base + reg) = value;
}

static uint32_t lapic_read(size_t reg) {
    return *(volatile uint32_t*)(g_lapic_base + reg);
}

uint32_t get_lapic_id() {
    return  lapic_read(XAPIC_ID_OFFSET) >> 24u;
}

err_t init_lapic() {
    err_t err = NO_ERROR;

    if (g_lapic_base == NULL) {
        g_madt = laihost_scan("APIC", 0);
        CHECK_ERROR_TRACE(g_madt != NULL, ERROR_NOT_FOUND, "Could not find APIC ACPI table");

        TRACE("Enabling APIC globally");
        MSR_IA32_APIC_BASE_REGISTER base = { .raw = __rdmsr(MSR_IA32_APIC_BASE) };
        g_lapic_base = PHYSICAL_TO_DIRECT(((uint32_t)base.ApicBase << 12u) | ((uint64_t)base.ApicBaseHi << 32u));
        base.EN = 1;
        __wrmsr(MSR_IA32_APIC_BASE, base.raw);
    }

    // get the apic id
    uint32_t id = get_lapic_id();
    TRACE("Configuring LAPIC #%d", id);

    // enable apic
    lapic_svr_t svr = { .raw = lapic_read(XAPIC_SPURIOUS_VECTOR_OFFSET) };
    svr.spurious_vector = 0xff;
    svr.software_enable = 1;
    lapic_write(XAPIC_SPURIOUS_VECTOR_OFFSET, svr.raw);

    // TODO: setup timer and shit

cleanup:
    return err;
}

static void send_ipi(lapic_icr_low_t low, uint8_t lapic_id) {
    critical_t critical;
    enter_critical(&critical);

    // send it
    lapic_write(XAPIC_ICR_HIGH_OFFSET, (uint32_t)lapic_id << 24);
    lapic_write(XAPIC_ICR_LOW_OFFSET, low.raw);

    // wait for finish
    do {
        low.raw = lapic_read(XAPIC_ICR_LOW_OFFSET);
    } while(low.delivery_status != 0);

    exit_critical(&critical);
}

static void send_init_ipi(uint32_t lapic_id) {
    lapic_icr_low_t icr = {
        .delivery_mode = LOCAL_APIC_DELIVERY_MODE_INIT,
        .level = 1,
    };
    send_ipi(icr, lapic_id);
}

static void send_sipi_ipi(uint32_t lapic_id, uint32_t entry) {
    lapic_icr_low_t icr = {
        .delivery_mode = LOCAL_APIC_DELIVERY_MODE_STARTUP,
        .level = 1,
        .vector = entry >> 12
    };
    send_ipi(icr, lapic_id);
}

void per_cpu_entry();

extern char g_smp_trampoline[];
extern size_t g_smp_trampoline_size;

err_t startup_all_cores() {
    err_t err = NO_ERROR;

    // setup everything for starting the cores
    memcpy(PHYSICAL_TO_DIRECT((void*)0x1000), g_smp_trampoline, g_smp_trampoline_size);
    POKE64(SMP_KERNEL_ENTRY) = (uintptr_t)per_cpu_entry;
    POKE64(SMP_KERNEL_PAGE_TABLE) = DIRECT_TO_PHYSICAL(g_kernel.address_space.pml4);
    POKE(idt_t, SMP_KERNEL_IDT) = g_idt;
    POKE(gdt_t, SMP_KERNEL_GDT) = g_gdt;

    // map the data and code
    CHECK_AND_RETHROW(vmm_map(&g_kernel.address_space, 0, 0, MAP_WRITE));
    CHECK_AND_RETHROW(vmm_map(&g_kernel.address_space, (directptr_t)0x1000, 0x1000, MAP_WRITE | MAP_EXEC));

    FOR_EACH_IN_MADT() {
        if (entry->type != MADT_LAPIC) continue;
        if (entry->lapic.apic_id == get_lapic_id() || (!entry->lapic.enabled && !entry->lapic.online_capable)) continue;

        POKE64(SMP_FLAG) = 0;
        POKE64(SMP_STACK_POINTER) = (uintptr_t)alloc_stack();
        // TODO: we need to free this stack somehow

        send_init_ipi(entry->lapic.apic_id);
        stall(10000);
        send_sipi_ipi(entry->lapic.apic_id, 0x1000);
        stall(1000);
        if (POKE64(SMP_FLAG) == 0) {
            send_sipi_ipi(entry->lapic.apic_id, 0x1000);
            stall(1000000);
            CHECK_TRACE(POKE64(SMP_FLAG) != 0, "Failed to initialize core #%u", entry->lapic.apic_id);
        }
    }

    // unmap these
    CHECK_AND_RETHROW(vmm_unmap(&g_kernel.address_space, 0));
    CHECK_AND_RETHROW(vmm_unmap(&g_kernel.address_space, (directptr_t)0x1000));

cleanup:
    return err;
}
