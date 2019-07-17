#ifndef TOMATKERNEL_TERM_H
#define TOMATKERNEL_TERM_H

#include <tboot/tboot.h>
#include <error.h>

// TODO Comment

void term_early_init(tboot_info_t* info);
void term_init();

void term_write(const char* str);
void term_clear();
void term_scroll();
void term_set_background_color(uint32_t col);
void term_set_foreground_color(uint32_t col);
uint32_t term_get_background_color();
uint32_t term_get_foreground_color();

#endif //TOMATKERNEL_TERM_H
