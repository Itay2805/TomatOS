#include "arch.h"

void io_write_8(uint16_t port, uint8_t data) {
    __asm__ __volatile__ ("outb %b0,%w1" : : "a" (data), "d" (port));
}

void io_write_16(uint16_t port, uint16_t data) {
    __asm__ __volatile__ ("outw %b0,%w1" : : "a" (data), "d" (port));
}

void io_write_32(uint16_t port, uint32_t data) {
    __asm__ __volatile__ ("outl %b0,%w1" : : "a" (data), "d" (port));
}

uint8_t io_read_8(uint16_t port) {
    uint8_t   data;
    __asm__ __volatile__ ("inb %w1,%b0" : "=a" (data) : "d" (port));
    return data;
}

uint16_t io_read_16(uint16_t port) {
    uint8_t   data;
    __asm__ __volatile__ ("inw %w1,%b0" : "=a" (data) : "d" (port));
    return data;
}

uint32_t io_read_32(uint16_t port) {
    uint8_t   data;
    __asm__ __volatile__ ("inl %w1,%b0" : "=a" (data) : "d" (port));
    return data;
}

uint64_t read_msr(uint32_t code) {
    uint32_t low, high;
    asm volatile("rdmsr" : "=a"(low), "=d"(high) : "c"(code));
    return ((uint64_t)high << 32u) | (uint64_t)low;
}

void write_msr(uint32_t code, uint64_t value) {
    asm volatile("wrmsr" : : "c"(code), "a"(value & 0xFFFFFFFF), "d"(value >> 32));
}

uint64_t read_cr0() {
    unsigned long val;
    asm volatile ( "mov %%cr3, %0" : "=r"(val) );
    return val;
}

void write_cr0(uint64_t value) {
    asm volatile ( "mov %0, %%cr0" : : "r"(value) );
}

uint64_t read_cr2() {
    unsigned long val;
    asm volatile ( "mov %%cr2, %0" : "=r"(val) );
    return val;
}

void write_cr2(uint64_t value) {
    asm volatile ( "mov %0, %%cr2" : : "r"(value) );
}

uint64_t read_cr3() {
    unsigned long val;
    asm volatile ( "mov %%cr0, %0" : "=r"(val) );
    return val;
}

void write_cr3(uint64_t value) {
    asm volatile ( "mov %0, %%cr3" : : "r"(value) );
}

uint64_t read_cr4() {
    unsigned long val;
    asm volatile ( "mov %%cr4, %0" : "=r"(val) );
    return val;
}

void write_cr4(uint64_t value) {
    asm volatile ( "mov %0, %%cr4" : : "r"(value) );
}

void cpuid(int code, int subcode, uint32_t data[4]) {
    asm volatile("cpuid"
        : "=a"(data[CPUID_EAX])
        , "=b"(data[CPUID_EBX])
        , "=c"(data[CPUID_ECX])
        , "=d"(data[CPUID_EDX])

        : "a"(code)
        , "c"(subcode)
    );
}

void read_idtr(idt_t* idtr) {
    __asm__ __volatile__ (
        "sidt  %0"
        : "=m" (*idtr)
    );
}

void write_idtr(idt_t* idtr) {
    __asm__ __volatile__ (
        "lidt %0"
        :
        : "m" (*idtr)
    );
}