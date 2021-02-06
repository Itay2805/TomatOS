#include <arch/io.h>
#include "cpu.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Port io operations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void io_write_8(uint16_t port, uint8_t value) {
    asm volatile ("outb %b0,%w1" : : "a"(value), "d"(port));
}

void io_write_16(uint16_t port, uint16_t value) {
    asm volatile ("outw %w0,%w1" : : "a"(value), "d"(port));
}

void io_write_32(uint16_t port, uint32_t value) {
    asm volatile ("outl %0,%w1" : : "a"(value), "d"(port));
}

uint8_t io_read_8(uint16_t port) {
    uint8_t value;
    __asm__ __volatile__ ("inb %w1,%b0" : "=a"(value) : "d"(port));
    return value;
}

uint16_t io_read_16(uint16_t port) {
    uint16_t value;
    asm volatile ("inw %w1,%w0" : "=a"(value) : "d"(port));
    return value;
}

uint32_t io_read_32(uint16_t port) {
    uint32_t value;
    asm volatile ("inl %w1,%0" : "=a"(value) : "d"(port));
    return value;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Memory IO operations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void mmio_write_8(uintptr_t addr, uint8_t value) {
    memory_barrier();
    *(volatile uint8_t*)addr = value;
    memory_barrier();
}

void mmio_write_16(uintptr_t addr, uint16_t value) {
    memory_barrier();
    *(volatile uint16_t*)addr = value;
    memory_barrier();
}

void mmio_write_32(uintptr_t addr, uint32_t value) {
    memory_barrier();
    *(volatile uint32_t*)addr = value;
    memory_barrier();
}

void mmio_write_64(uintptr_t addr, uint64_t value) {
    memory_barrier();
    *(volatile uint64_t*)addr = value;
    memory_barrier();
}

uint8_t mmio_read_8(uintptr_t addr) {
    memory_barrier();
    uint8_t value = *(volatile uint8_t*)addr;
    memory_barrier();
    return value;
}

uint16_t mmio_read_16(uintptr_t addr) {
    memory_barrier();
    uint16_t value = *(volatile uint16_t*)addr;
    memory_barrier();
    return value;
}

uint32_t mmio_read_32(uintptr_t addr) {
    memory_barrier();
    uint32_t value = *(volatile uint32_t*)addr;
    memory_barrier();
    return value;
}

uint64_t mmio_read_64(uintptr_t addr) {
    memory_barrier();
    uint64_t value = *(volatile uint64_t*)addr;
    memory_barrier();
    return value;
}
