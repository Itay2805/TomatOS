#include <mm/mm.h>
#include <arch/intrin.h>
#include <arch/msr.h>
#include "pcpu.h"

extern void* kernel_pcpu_data_size;
#define KERNEL_PCPU_DATA_SIZE ((size_t)(&kernel_pcpu_data_size))

extern uintptr_t bsp_pcpu_data[];

CPU_LOCAL uint64_t current_cpu_index;

void pcpu_init_for_cpu(uint64_t num) {
    void* data = mm_allocate(KERNEL_PCPU_DATA_SIZE);
    __writemsr(MSR_IA32_GS_BASE, (uintptr_t)data);
    current_cpu_index = num;
}

void pcpu_init_for_bsp() {
    __writemsr(MSR_IA32_GS_BASE, (uintptr_t)bsp_pcpu_data);
    current_cpu_index = 0;
}

uintptr_t get_pcpu_base() {
    return __readmsr(MSR_IA32_GS_BASE);
}
