#ifndef TOMATOS_IO_H
#define TOMATOS_IO_H

#include <stdint.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Port io operations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void io_write_8(uint16_t port, uint8_t value);
void io_write_16(uint16_t port, uint16_t value);
void io_write_32(uint16_t port, uint32_t value);

uint8_t io_read_8(uint16_t port);
uint16_t io_read_16(uint16_t port);
uint32_t io_read_32(uint16_t port);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Memory IO operations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void mmio_write_8(uintptr_t addr, uint8_t value);
void mmio_write_16(uintptr_t addr, uint16_t value);
void mmio_write_32(uintptr_t addr, uint32_t value);
void mmio_write_64(uintptr_t addr, uint64_t value);

uint8_t mmio_read_8(uintptr_t addr);
uint16_t mmio_read_16(uintptr_t addr);
uint32_t mmio_read_32(uintptr_t addr);
uint64_t mmio_read_64(uintptr_t addr);

#endif //TOMATOS_IO_H
