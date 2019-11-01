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
