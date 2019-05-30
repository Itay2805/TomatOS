#ifndef TOMATKERNEL_PORTIO_H
#define TOMATKERNEL_PORTIO_H

#include <stdint.h>

static inline void outb(uint16_t port, uint8_t data) {
    asm volatile ("outb %0, %1" : : "a"(data), "Nd"(port));
}

static inline void outw(uint16_t port, uint16_t data) {
    asm volatile ("outw %0, %1" : : "a"(data), "Nd"(port));
}

static inline void outl(uint16_t port, uint32_t data) {
    asm volatile ("outl %0, %1" : : "a"(data), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t result = 0;
    asm volatile ("inb %1, %0" : "=a" (result) : "Nd" (port));
    return result;
}

static inline uint16_t inw(uint16_t port) {
    uint16_t result = 0;
    asm volatile ("inw %1, %0" : "=a" (result) : "Nd" (port));
    return result;
}

static inline uint32_t inl(uint16_t port) {
    uint32_t result = 0;
    asm volatile ("inl %1, %0" : "=a" (result) : "Nd" (port));
    return result;
}

#endif //TOMATKERNEL_PORTIO_H
