#ifndef TOMATKERNEL_TEXT_MODE_H
#define TOMATKERNEL_TEXT_MODE_H

#include <boot/boot.h>
#include <stdint.h>

void text_init(boot_info_t* info);
void text_write(const char*);
void text_clear();
void text_scroll(uint16_t n);

#endif //TOMATKERNEL_TEXT_MODE_H
