#ifndef TOMATOS_REGISTERS_H
#define TOMATOS_REGISTERS_H

#include <common.h>
#include <stdint.h>

#define GENERATE_REGISTER_READ_WRITE(dev_type, mmio_field) \
    static inline uint8_t read8_relaxed(dev_type* dev, size_t offset) { return POKE8(((uintptr_t)(dev)->mmio_field) + offset); } \
    static inline uint16_t read16_relaxed(dev_type* dev, size_t offset) { return POKE16(((uintptr_t)(dev)->mmio_field) + offset); } \
    static inline uint32_t read32_relaxed(dev_type* dev, size_t offset) { return POKE32(((uintptr_t)(dev)->mmio_field) + offset); } \
    static inline uint64_t read64_relaxed(dev_type* dev, size_t offset) { return POKE64(((uintptr_t)(dev)->mmio_field) + offset); } \
    static inline uint8_t read8(dev_type* dev, size_t offset) { return VOL_POKE8(((uintptr_t)(dev)->mmio_field) + offset); } \
    static inline uint16_t read16(dev_type* dev, size_t offset) { return VOL_POKE16(((uintptr_t)(dev)->mmio_field) + offset); } \
    static inline uint32_t read32(dev_type* dev, size_t offset) { return VOL_POKE32(((uintptr_t)(dev)->mmio_field) + offset); } \
    static inline uint64_t read64(dev_type* dev, size_t offset) { return VOL_POKE64(((uintptr_t)(dev)->mmio_field) + offset); } \
    static inline void write8_relaxed(dev_type* dev, size_t offset, uint8_t data) { POKE8(((uintptr_t)(dev)->mmio_field) + offset) = data; } \
    static inline void write16_relaxed(dev_type* dev, size_t offset, uint16_t data) { POKE16(((uintptr_t)(dev)->mmio_field) + offset) = data; } \
    static inline void write32_relaxed(dev_type* dev, size_t offset, uint32_t data) { POKE32(((uintptr_t)(dev)->mmio_field) + offset) = data; } \
    static inline void write64_relaxed(dev_type* dev, size_t offset, uint64_t data) { POKE64(((uintptr_t)(dev)->mmio_field) + offset) = data; } \
    static inline void write8(dev_type* dev, size_t offset, uint8_t data) { VOL_POKE8(((uintptr_t)(dev)->mmio_field) + offset) = data; } \
    static inline void write16(dev_type* dev, size_t offset, uint16_t data) { VOL_POKE16(((uintptr_t)(dev)->mmio_field) + offset) = data; } \
    static inline void write32(dev_type* dev, size_t offset, uint32_t data) { VOL_POKE32(((uintptr_t)(dev)->mmio_field) + offset) = data; } \
    static inline void write64(dev_type* dev, size_t offset, uint64_t data) { VOL_POKE64(((uintptr_t)(dev)->mmio_field) + offset) = data; }

#endif //TOMATOS_REGISTERS_H
