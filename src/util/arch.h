#ifndef TOMATKERNEL_IO_H
#define TOMATKERNEL_IO_H

#include <stdint.h>

void io_write_8(uint16_t port, uint8_t data);
void io_write_16(uint16_t port, uint16_t data);
void io_write_32(uint16_t port, uint32_t data);

uint8_t io_read_8(uint16_t port);
uint16_t io_read_16(uint16_t port);
uint32_t io_read_32(uint16_t port);

#endif //TOMATKERNEL_IO_H
