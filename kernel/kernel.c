#include "kernel.h"

#include <memory/pmm.h>
#include <memory/vmm.h>
#include <common/string.h>
#include <interrupts/idt.h>
#include <interrupts/irq.h>
#include <interrupts/isr.h>


#include "graphics/term.h"

extern void* boot_pdpe;

#define PRESENT_BIT ((uint64_t)1 << (uint64_t)0)
#define READ_WRITE_BIT ((uint64_t)1 << (uint64_t)1)
#define PAGE_SIZE_BIT ((uint64_t)1 << (uint64_t)7)
#define PAGING_1GB_PDPE_MASK    ((uint64_t)0x7FFFFFFFC0000000)

void kernel_main(multiboot_info_t* info) {
    // identity map first 4GB
    // should be enough to make sure everything will be in a reachable place
    uint64_t* pdpe = (uint64_t*)&boot_pdpe;
    for(size_t i = 0; i < 4; i++) {
        uint64_t addr = 0x40000000 * i;
        pdpe[i] = (addr & PAGING_1GB_PDPE_MASK) | (PRESENT_BIT | READ_WRITE_BIT | PAGE_SIZE_BIT);
    }

    term_init(info);
    term_print("[kernel_main] successfully loaded the kernel (bootloader='%s', cmdline='%s')\n", (char *)(uintptr_t)info->boot_loader_name, (char *)(uintptr_t)info->cmdline);

    pmm_init(info);
    vmm_init(info);

    irq_init();
    isr_init();
    idt_init();
    sti();
}
