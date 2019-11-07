#include <acpi/tables/madt.h>
#include <memory/pmm.h>
#include <util/arch.h>
#include <memory/vmm.h>
#include <libc/string.h>
#include <interrupts/apic/lapic.h>
#include <memory/mm.h>
#include "percpu_storage.h"

static per_cpu_storage_t* per_cpu_storage;

void percpu_storage_init() {
    debug_log("[*] Preparing per cpu storage\n");

    // count the lapics, will tell us how much space we need to allocate
    // for per cpu storage
    int lapic_count = 0;
    for(int i = 0; ; i++) {
        madt_lapic_t* lapic = madt_get_next(MADT_TYPE_LAPIC, i);
        if(lapic == NULL) break;
        if(!lapic->processor_enabled) continue;

        if(lapic_count < lapic->id) {
            lapic_count = lapic->id;
        }
    }
    lapic_count++;

    // allocate and clear the per cpu storage
    size_t size = sizeof(per_cpu_storage_t) * lapic_count;
    pmm_allocate_pages(ALLOCATE_ANY, MEM_KERNEL_DATA, SIZE_TO_PAGES(size), (uintptr_t*)&per_cpu_storage);
    per_cpu_storage = PHYSICAL_TO_DIRECT(per_cpu_storage);
    memset(per_cpu_storage, 0, size);

    // initialize all the different entries per cpu
    for(int i = 0; ; i++) {
        madt_lapic_t* lapic = madt_get_next(MADT_TYPE_LAPIC, i);
        if(lapic == NULL) break;
        if(!lapic->processor_enabled) continue;

        per_cpu_storage_t* cur = &per_cpu_storage[lapic->id];
        cur->lapic_id = lapic->id;
        cur->processor_id = lapic->processor_id;

        // allocate stacks
        cur->interrupt_stack = mm_allocate_pages(1);
        cur->page_fault_stack = mm_allocate_pages(1);
        cur->exception_stack = mm_allocate_pages(1);
        cur->nmi_stack = mm_allocate_pages(1);

        // clear the stacks
        memset((void *) cur->interrupt_stack, 0, PAGE_SIZE);
        memset((void *) cur->page_fault_stack, 0, PAGE_SIZE);
        memset((void *) cur->exception_stack, 0, PAGE_SIZE);
        memset((void *) cur->nmi_stack, 0, PAGE_SIZE);

        // stack needs to be at the end
        cur->interrupt_stack += PAGE_SIZE;
        cur->page_fault_stack += PAGE_SIZE;
        cur->exception_stack += PAGE_SIZE;
        cur->nmi_stack += PAGE_SIZE;

        // set tss
        cur->tss.rsp0 = cur->interrupt_stack;
        cur->tss.rsp1 = (uint64_t) -1;
        cur->tss.rsp2 = (uint64_t) -1;
        cur->tss.ist1 = cur->exception_stack;
        cur->tss.ist2 = cur->page_fault_stack;
        cur->tss.ist3 = cur->nmi_stack;
        cur->tss.ist4 = (uint64_t) -1;
        cur->tss.ist5 = (uint64_t) -1;
        cur->tss.ist6 = (uint64_t) -1;
        cur->tss.ist7 = (uint64_t) -1;
        cur->tss.iopb_offset = sizeof(tss64_t);
    }
}

per_cpu_storage_t* get_percpu_storage() {
    ASSERT(lapic_get_id() != (uint8_t)-1);
    return &per_cpu_storage[lapic_get_id()];
}

per_cpu_storage_t* get_percpu_storage_of(int lapic_id) {
    return &per_cpu_storage[lapic_id];
}
