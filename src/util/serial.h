#ifndef __UTIL_SERIAL_H__
#define __UTIL_SERIAL_H__

#include <stddef.h>
#include <stdbool.h>

void serial_init();

void serial_write(const char* buffer, size_t size);

void serial_read(char* buffer, size_t bytes);

bool serial_poll();

#endif //__UTIL_SERIAL_H__
