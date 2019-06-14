#ifndef TOMATKERNEL_TEXT_MODE_H
#define TOMATKERNEL_TEXT_MODE_H

#include <boot/multiboot.h>
#include <stdint.h>

void text_early_init(multiboot_info_t* info);
void text_init();

void text_write(const char*);
void text_clear();
void text_scroll(uint16_t n);

#endif //TOMATKERNEL_TEXT_MODE_H
