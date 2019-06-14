#ifndef TOMATKERNEL_TERM_H
#define TOMATKERNEL_TERM_H

#include <error.h>
#include <boot/multiboot.h>

typedef void(*term_init_f)();
typedef void(*term_write_f)(const char* str);
typedef void(*term_clear_f)();
typedef void(*term_scroll_f)(uint16_t n);
typedef void(*term_set_background_color_f)(uint32_t col);
typedef void(*term_set_foreground_color_f)(uint32_t col);
typedef uint32_t (*term_get_background_color_f)();
typedef uint32_t (*term_get_foreground_color_f)();

/**
 * Early initialization of the terminal
 *
 * Will just device which provider to use
 */
void term_early_init(multiboot_info_t* info);

extern term_init_f term_init;
extern term_write_f term_write;
extern term_clear_f term_clear;
extern term_scroll_f term_scroll;
extern term_set_background_color_f term_set_background_color;
extern term_set_foreground_color_f term_set_foreground_color;
extern term_get_background_color_f term_get_background_color;
extern term_get_foreground_color_f term_get_foreground_color;


#endif //TOMATKERNEL_TERM_H
