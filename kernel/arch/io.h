#ifndef TOMATOS_IO_H
#define TOMATOS_IO_H

#include <stdint.h>

void io_write_8(uint16_t port, uint8_t value);
void io_write_16(uint16_t port, uint16_t value);
void io_write_32(uint16_t port, uint32_t value);

uint8_t io_read_8(uint16_t port);
uint16_t io_read_16(uint16_t port);
uint32_t io_read_32(uint16_t port);

#endif //TOMATOS_IO_H
