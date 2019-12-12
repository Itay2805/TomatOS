#include "io.hpp"

namespace arch::io {

    void write8(uint16_t port, uint8_t data) {
        __asm__ __volatile__ ("outb %b0,%w1" : : "a" (data), "d" (port));
    }

    void write16(uint16_t port, uint16_t data) {
        __asm__ __volatile__ ("outw %w0,%w1" : : "a" (data), "d" (port));
    }

    void write32(uint16_t port, uint32_t data) {
        __asm__ __volatile__ ("outl %0,%w1" : : "a" (data), "d" (port));
    }

    uint8_t read8(uint16_t port) {
        uint8_t   data;
        __asm__ __volatile__ ("inb %w1,%b0" : "=a" (data) : "d" (port));
        return data;
    }

    uint16_t read16(uint16_t port) {
        uint16_t   data;
        __asm__ __volatile__ ("inw %w1,%w0" : "=a" (data) : "d" (port));
        return data;
    }

    uint32_t read32(uint16_t port) {
        uint32_t   data;
        __asm__ __volatile__ ("inl %w1,%0" : "=a" (data) : "d" (port));
        return data;
    }

}
