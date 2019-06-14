#ifndef TOMATKERNEL_GRAPHICS_H
#define TOMATKERNEL_GRAPHICS_H

#include <boot/multiboot.h>
#include <stdint.h>

void graphics_early_init(multiboot_info_t* info);
void graphics_init();

void graphics_write(const char*);
void graphics_clear();
void graphics_scroll(uint16_t n);

#endif //TOMATKERNEL_GRAPHICS_H
