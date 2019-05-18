#ifndef TOMATKERNEL_PS2_H
#define TOMATKERNEL_PS2_H

#include <common/stdint.h>

#define PS2_FIRST   0
#define PS2_SECOND  1

error_t ps2_init();

bool ps2_poll();
error_t ps2_read(uint8_t* value);
error_t ps2_write(int port, uint8_t value);

#endif //TOMATKERNEL_PS2_H
