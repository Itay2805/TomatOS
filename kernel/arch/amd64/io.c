#include <arch/io.h>

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

