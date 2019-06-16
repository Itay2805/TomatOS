#ifndef TOMATKERNEL_GRAPHICS_H
#define TOMATKERNEL_GRAPHICS_H

#include <boot/boot.h>
#include <stdint.h>

void graphics_init(boot_info_t* info);
void graphics_write(const char*);
void graphics_clear();
void graphics_scroll(uint16_t n);
void graphics_set_background_color(uint32_t col);
void graphics_set_foreground_color(uint32_t col);
uint32_t graphics_get_background_color();
uint32_t graphics_get_foreground_color();

#endif //TOMATKERNEL_GRAPHICS_H
