#include <acpi/tables/rsdt.h>
#include <acpi/tables/fadt.h>
#include <processes/process.h>

#include <memory/vmm.h>
#include <memory/mm.h>
#include <util/arch.h>

#include <stddef.h>
#include <string.h>

#include <lai/host.h>


void* laihost_malloc(size_t size) {
    return mm_pool_allocate(size);
}

void* laihost_realloc(void* ptr, size_t size) {
    return mm_pool_realloc(ptr, size);
}

void laihost_free(void* ptr) {
    mm_pool_free(ptr);
}

void laihost_log(int level, const char* msg) {
    switch(level) {
        case LAI_WARN_LOG: debug_log("[!] lai: %s\n", msg); break;
        case LAI_DEBUG_LOG:  debug_log("[*] lai: %s\n", msg); break;
        default: ASSERT(false);
    }
}

void laihost_panic(const char* msg) {
    debug_log("[-] lai: %s\n", msg);
    ASSERT(false);
}

void* laihost_map(size_t base, size_t size) {
    if(!vmm_is_mapped(&kernel_process->vmm_handle, base, size)) {
        vmm_map(&kernel_process->vmm_handle, base, PHYSICAL_TO_DIRECT(base), size, PAGE_SUPERVISOR_READWRITE, DEFAULT_CACHE);
    }
    return (void*)PHYSICAL_TO_DIRECT(base);
}

void* laihost_scan(const char* sig, size_t index) {

    if(strcmp(sig, "DSDT") == 0 && index == 0) {
        return acpi_dsdt;
    }else {
        void* table = rsdt_search(sig, index);
        return table;
    }

    ASSERT(false);
}

void laihost_outb(uint16_t port, uint8_t value) {
    io_write_8(port, value);
}

void laihost_outw(uint16_t port, uint16_t value) {
    io_write_16(port, value);
}

void laihost_outd(uint16_t port, uint32_t value) {
    io_write_32(port, value);
}

uint8_t laihost_inb(uint16_t port) {
    return io_read_8(port);
}

uint16_t laihost_inw(uint16_t port) {
    return io_read_16(port);
}

uint32_t laihost_ind(uint16_t port) {
    return io_read_32(port);
}

// TODO: PCI access

void laihost_sleep(uint64_t millis) {
    // TODO: implement this
    debug_log("[!] TODO laihost_sleep!\n");
}
