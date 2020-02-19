#include <mm/mm.h>
#include <arch/intrin.h>
#include <arch/msr.h>
#include "pcpu.h"

extern void* kernel_pcpu_data_size;
#define KERNEL_PCPU_DATA_SIZE ((size_t)(&kernel_pcpu_data_size))

extern uintptr_t bsp_pcpu_data[];

void pcpu_init_for_cpu() {
    void* data = mm_allocate(KERNEL_PCPU_DATA_SIZE);
    __writemsr(MSR_CODE_IA32_GS_BASE, (uintptr_t)data);
}

void pcpu_init_for_bsp() {
    __writemsr(MSR_CODE_IA32_GS_BASE, (uintptr_t)bsp_pcpu_data);
}

uintptr_t get_pcpu_base() {
    return __readmsr(MSR_CODE_IA32_GS_BASE);
}
