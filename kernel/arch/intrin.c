#include "intrin.h"

uint64_t __readcr8(void) {
    uint64_t value;
    __asm__ __volatile__ (
    "mov %%cr8, %[value]"
    : [value] "=q" (value));
    return value;
}

void __writecr8(uint64_t Data) {
    __asm__ __volatile__ (
    "mov %[Data], %%cr8"
    :
    : [Data] "q" (Data)
    : "memory");
}

IA32_CR4 __readcr4(void) {
    uint64_t value;
    __asm__ __volatile__ (
    "mov %%cr4, %[value]"
    : [value] "=q" (value));
    return (IA32_CR4) { .raw = value };
}

void __writecr4(IA32_CR4 Data) {
    __asm__ __volatile__ (
    "mov %[Data], %%cr4"
    :
    : [Data] "q" (Data.raw)
    : "memory");
}

uint64_t __readcr3(void) {
    uint64_t value;
    __asm__ __volatile__ (
    "mov %%cr3, %[value]"
    : [value] "=q" (value));
    return value;
}

void __writecr3(uint64_t Data) {
    __asm__ __volatile__ (
    "mov %[Data], %%cr3"
    :
    : [Data] "q" (Data)
    : "memory");
}

uint64_t __readcr2(void) {
    uint64_t value;
    __asm__ __volatile__ (
    "mov %%cr2, %[value]"
    : [value] "=q" (value));
    return value;
}

void __writecr2(uint64_t Data) {
    __asm__ __volatile__ (
    "mov %[Data], %%cr2"
    :
    : [Data] "q" (Data)
    : "memory");
}

IA32_CR0 __readcr0(void) {
    uint64_t value;
    __asm__ __volatile__ (
    "mov %%cr0, %[value]"
    : [value] "=q" (value));
    return (IA32_CR0) { .raw = value };
}

void __writecr0(IA32_CR0 Data) {
    __asm__ __volatile__ (
    "mov %[Data], %%cr0"
    :
    : [Data] "q" (Data.raw)
    : "memory");
}

uint64_t __rdmsr(uint32_t msr) {
    uint32_t val1, val2;
    __asm__ __volatile__(
    "rdmsr"
    : "=a" (val1), "=d" (val2)
    : "c" (msr));
    return ((uint64_t) val1) | (((uint64_t)val2) << 32u);
}

void __wrmsr (uint32_t msr, uint64_t Value) {
    uint32_t val1 = Value, val2 = Value >> 32;
    __asm__ __volatile__ (
    "wrmsr"
    :
    : "c" (msr), "a" (val1), "d" (val2));
}

void __cpuid(int __info[4], int __level) {
    __asm__ ("cpuid" : "=a"(__info[0]), "=b" (__info[1]), "=c"(__info[2]), "=d"(__info[3])
    : "a"(__level), "c"(0));
}
void __cpuidex(int __info[4], int __level, int __ecx) {
    __asm__ ("cpuid" : "=a"(__info[0]), "=b" (__info[1]), "=c"(__info[2]), "=d"(__info[3])
    : "a"(__level), "c"(__ecx));
}

void cpuid(uint32_t val, uint32_t* rax, uint32_t* rbx, uint32_t* rcx, uint32_t* rdx) {
    int arr[4] = {0};
    __cpuid(arr, val);
    if (rax) {
        *rax = arr[0];
    }
    if (rbx) {
        *rbx = arr[1];
    }
    if (rcx) {
        *rcx = arr[2];
    }
    if (rdx) {
        *rdx = arr[3];
    }
}

void cpuidex(uint32_t val, uint64_t leaf, uint32_t* rax, uint32_t* rbx, uint32_t* rcx, uint32_t* rdx) {
    int arr[4] = {0};
    __cpuidex(arr, val, leaf);
    if (rax) {
        *rax = arr[0];
    }
    if (rbx) {
        *rbx = arr[1];
    }
    if (rcx) {
        *rcx = arr[2];
    }
    if (rdx) {
        *rdx = arr[3];
    }
}

void __invlpg(uintptr_t a) {
    __asm__ __volatile__("invlpg (%%eax)" : : "a" (a) );
}

void __hlt(void) {
    __asm__ volatile ("hlt");
}

void __nop(void) {
    __asm__ volatile ("nop");
}

void __stac(void) {
    __asm__ volatile ("stac" ::: "cc");
}

void __clac(void) {
    __asm__ volatile ("clac" ::: "cc");
}

void __ltr(uint16_t seg){
    asm volatile("ltr %%ax" : : "a"(seg));
}

void __lgdt(gdt_t* gdt) {
    asm volatile ( "lgdt %0" : : "m" (*gdt));
}

void __swapgs(void) {
    asm volatile("swapgs" ::: "memory");
}

uint64_t __xgetbv(uint32_t index) {
    uint32_t eax, edx;
    asm volatile("xgetbv" : "=a" (eax), "=d" (edx) : "c" (index));
    return eax + ((uint64_t)edx << 32);
}

void __xsetbv(uint32_t index, uint64_t value) {
    uint32_t eax = value;
    uint32_t edx = value >> 32;
    asm volatile("xsetbv" : : "a" (eax), "d" (edx), "c" (index));
}

void __fxsave(void* state) {
    asm volatile ("fxsave %0" : : "m"(*(uint8_t*)state) : "memory");
}

void __fxrstor(void* state) {
    asm volatile ("fxrstor %0" : : "m"(*(uint8_t*)state) : "memory");
}

void __xsave(void* state) {
    asm volatile ("xsave %0" : : "m"(*(uint8_t*)state), "a" (0xFFFFFFFF), "d" (0xFFFFFFFF) : "memory");
}

void __xrstor(void* state) {
    asm volatile ("xrstor %0" : : "m"(*(uint8_t*)state), "a" (0xFFFFFFFF), "d" (0xFFFFFFFF) : "memory");
}

void __monitor(size_t eax, size_t ecx, size_t edx) {
    asm volatile ("monitor" : : "a"(eax), "c"(ecx), "d"(edx));
}

void __mwait(size_t eax, size_t ecx) {
    asm volatile ("mwait" : : "a"(eax), "c"(ecx));
}
