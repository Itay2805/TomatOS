#include <acpi/tables/rsdp.h>
#include <acpi/tables/rsdt.h>
#include <arch/timing.h>
#include <util/except.h>
#include <lai/host.h>
#include <arch/cpu.h>
#include <mem/mm.h>
#include <proc/process.h>
#include <util/stb_ds.h>
#include <proc/event.h>
#include <arch/ints.h>
#include <lai/helpers/sci.h>
#include <proc/scheduler.h>
#include <stdnoreturn.h>

#include "acpi.h"

#define ACPI_TIMER_FREQUENCY (3579545)

/**
 * The timer block of the timer
 */
static uint16_t g_timer_port;

/**
 * 32bit counter is used
 */
static bool g_extended_timer = false;

/**
 * The overflow value
 */
static uint32_t g_timer_mask = 0;

/**
 * The total amount of overflows we had
 */
static _Atomic(uint64_t) g_total_ticks = 0;

/**
 * The counter when we actually started counting
 */
static uint32_t g_initial_ticks = 0;

static acpi_fadt_t* g_acpi_fadt;

memmap_entry_t* g_memory_map = NULL;

static uint32_t get_timer_ticks() {
    return laihost_ind(g_timer_port) & g_timer_mask;
}

void init_acpi_tables(uintptr_t rsdp_ptr) {
    TRACE("ACPI Table initialization");
    rsdp_init(rsdp_ptr);
    rsdt_init();
    lai_set_acpi_revision(g_acpi_rsdp->revision);

    // initialize the pm timer
    g_acpi_fadt = (acpi_fadt_t*)rsdt_search("FACP", 0);
    ASSERT_TRACE(g_acpi_fadt != NULL, "The FADT must be present");

    // initialize timer
    ASSERT_TRACE(g_acpi_fadt->pm_timer_length == 4, "ACPI Timer must be present");
    TRACE("Using ACPI Timer");
    if (g_acpi_rsdp->revision >= 2 && g_acpi_fadt->x_pm_timer_block.base != 0) {
        if (g_acpi_fadt->x_pm_timer_block.address_space != ACPI_GAS_IO) {
            TRACE("TODO: support timer of type %d", g_acpi_fadt->x_pm_timer_block.address_space);
            g_timer_port = g_acpi_fadt->pm_timer_block;
        } else {
            g_timer_port = g_acpi_fadt->x_pm_timer_block.base;
            ASSERT(g_acpi_fadt->x_pm_timer_block.bit_offset == 0);
            ASSERT(g_acpi_fadt->x_pm_timer_block.access_size == 4);
            ASSERT(g_acpi_fadt->x_pm_timer_block.bit_width >= 24);
        }
    } else {
        g_timer_port = g_acpi_fadt->pm_timer_block;
    }

    // initialize timer values
    if (g_acpi_fadt->flags & BIT8) {
        TRACE("\t* Extended timer is supported");
        g_extended_timer = true;
        g_timer_mask = UINT32_MAX;
    } else {
        g_timer_mask = 0xFFFFFF;
    }

    g_initial_ticks = get_timer_ticks();
}

static void acpi_delay(uint64_t delay) {
    uint32_t times = delay >> (g_extended_timer ? 30 : 22);
    times &= (g_extended_timer ? BIT30 : BIT22) - 1;
    do {
        uint32_t ticks = get_timer_ticks() + delay;
        delay = g_extended_timer ? BIT30 : BIT22;

        uint32_t bit = g_extended_timer ? BIT31 : BIT23;
        while (((ticks - get_timer_ticks()) & bit) == 0) {
            cpu_pause();
        }
    } while (times-- > 0);
}

/**
 * The vector used for scis
 */
static uint8_t g_sci_vector = 0;

/**
 * The acpi event handling thread
 */
static thread_t* g_acpi_thread = NULL;

/**
 * This thread handles scis
 */
static noreturn void acpi_thread() {
    TRACE("ACPI thread started (sci=%d)", g_sci_vector);

    while (true) {
        wait_for_interrupt(g_sci_vector);

        uint16_t event = lai_get_sci_event();

        if (event & ACPI_TIMER) {
            bool really_overflowed = !(get_timer_ticks() & (g_extended_timer ? BIT31 : BIT23));
            if (really_overflowed) {
                g_total_ticks += g_timer_mask;
                g_initial_ticks = 0;
            }
        }

        if (event & ACPI_POWER_BUTTON) {

        }
    }
}

err_t init_acpi() {
    err_t err = NO_ERROR;

    TRACE("Initializing ACPI");
    lai_create_namespace();
    lai_enable_acpi(1);

    // setup the interrupts we want
    lai_set_sci_event(ACPI_POWER_BUTTON | ACPI_SLEEP_BUTTON | ACPI_WAKE | ACPI_TIMER);
    lai_get_sci_event();

    // do sci handling
    CHECK_AND_RETHROW(register_irq(g_acpi_fadt->sci_irq, &g_sci_vector));
    CHECK_AND_RETHROW(create_thread(&g_acpi_thread, acpi_thread, NULL, "acpi"));
    schedule_thread(g_acpi_thread);

cleanup:
    return err;
}

void ustall(uint64_t ns) {
    acpi_delay((ns * ACPI_TIMER_FREQUENCY) / 1000000000u);
}

void stall(uint64_t ms) {
    acpi_delay((ms * ACPI_TIMER_FREQUENCY) / 1000000u);
}

uint64_t uptime() {
    return (((g_total_ticks + get_timer_ticks()) - g_initial_ticks) * 1000000u) / ACPI_TIMER_FREQUENCY;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Laihost functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

char buffer;

void* laihost_malloc(size_t size) {
    if (size == 0) {
        return &buffer;
    }
    return kalloc(size);
}

void* laihost_realloc(void* ptr, size_t size) {
    return krealloc(ptr, size);
}

void laihost_free(void* ptr) {
    kfree(ptr);
}

void* laihost_map(size_t address, size_t count) {
    for (int i = 0; i < arrlen(g_memory_map); i++) {
        if (g_memory_map[i].base <= address && g_memory_map[i].end >= address + count) {
            if (g_memory_map[i].acpi_access) {
                return PHYSICAL_TO_DIRECT(address);
            } else {
                ASSERT_TRACE(false, "ACPI tried to access an invalid address (%p - %p)", address, address + count);
            }
        }
    }

    // if not in memory map assume mmio and that acpi can access it,
    // also map it for acpi
    ASSERT_TRACE(address + count <= arrlast(g_memory_map).end, "TODO: map unmapped addresses");
    return PHYSICAL_TO_DIRECT(address);
}

void laihost_unmap(void *pointer, size_t count) {

}

void laihost_log(int level, const char* msg) {
    TRACE("%s", msg);
}

void laihost_panic(const char * fmt) {
    ASSERT_TRACE(false, "%s", fmt);
}

void* laihost_scan(const char* signature, size_t index) {
    if (memcmp(signature, "DSDT", 4) == 0) {
        if (g_acpi_fadt->x_dsdt) {
            return PHYSICAL_TO_DIRECT((physptr_t)g_acpi_fadt->dsdt);
        } else {
            return PHYSICAL_TO_DIRECT(g_acpi_fadt->x_dsdt);
        }
    } else {
        return rsdt_search(signature, index);
    }
}

#ifdef __TOMATOS_AMD64__

#include <arch/amd64/intrin.h>
#include <sys/pci/pci.h>

void laihost_outb(uint16_t port, uint8_t value) {
    __outb(port, value);
}

void laihost_outw(uint16_t port, uint16_t value) {
    __outw(port, value);
}

void laihost_outd(uint16_t port, uint32_t value) {
    __outl(port, value);
}

uint8_t laihost_inb(uint16_t port) {
    return __inb(port);
}

uint16_t laihost_inw(uint16_t port) {
    return __inw(port);
}

uint32_t laihost_ind(uint16_t port) {
    return __inl(port);
}

// TODO: pci

#endif

void laihost_sleep(uint64_t ms) {
    stall(ms * 1000);
}

void laihost_pci_writeb(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset, uint8_t val) {
    void* region;
    ASSERT(!IS_ERROR(get_config_for_pci(seg, bus, slot, fun, &region)));
    *(uint8_t*)(region + offset) = val;
}

void laihost_pci_writew(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset, uint16_t val) {
    void* region;
    ASSERT(!IS_ERROR(get_config_for_pci(seg, bus, slot, fun, &region)));
    *(uint16_t*)(region + offset) = val;
}

void laihost_pci_writed(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset, uint32_t val) {
    void* region;
    ASSERT(!IS_ERROR(get_config_for_pci(seg, bus, slot, fun, &region)));
    *(uint32_t*)(region + offset) = val;
}


uint8_t laihost_pci_readb(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset) {
    void* region;
    ASSERT(!IS_ERROR(get_config_for_pci(seg, bus, slot, fun, &region)));
    struct PACKED {
        uint8_t val;
    }* ptr = region + offset;
    return ptr->val;
}

uint16_t laihost_pci_readw(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset) {
    void* region;
    ASSERT(!IS_ERROR(get_config_for_pci(seg, bus, slot, fun, &region)));
    struct PACKED {
        uint16_t val;
    }* ptr = region + offset;
    return ptr->val;
}

uint32_t laihost_pci_readd(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset) {
    void* region;
    ASSERT(!IS_ERROR(get_config_for_pci(seg, bus, slot, fun, &region)));
    struct PACKED {
        uint32_t val;
    }* ptr = region + offset;
    return ptr->val;
}