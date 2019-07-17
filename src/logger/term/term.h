#ifndef TOMATKERNEL_TERM_H
#define TOMATKERNEL_TERM_H

#include <tboot/tboot.h>
#include <error.h>

// TODO Comment

/**
 * Will initialize the framebuffer logger
 *
 * @remark
 * Must be done after the memory manager have been initialized!
 */
void term_init(tboot_info_t* info);

void term_write(const char* str);
void term_clear();
void term_scroll();
void term_set_background_color(uint32_t col);
void term_set_foreground_color(uint32_t col);
uint32_t term_get_background_color();
uint32_t term_get_foreground_color();

void term_disable();

#endif //TOMATKERNEL_TERM_H
