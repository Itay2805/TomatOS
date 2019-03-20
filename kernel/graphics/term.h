#ifndef GRAPHICS_TERM_H
#define GRAPHICS_TERM_H

////////////////////////////////////////////////////////////////////////////
// Terminal
////////////////////////////////////////////////////////////////////////////
//
// This is a terminal implementation, it is basically just so in the early
// initialization stages we will have a way to give information on what we
// are doing. 
// 
// Note: this terminal is designed to work in a graphics mode. In the future
//       I should add a check that gives a warning if no graphics mode is
//       supported...
//
////////////////////////////////////////////////////////////////////////////

#include "../common/multiboot.h"
#include "../common/stdint.h"

////////////////////////////////////////////////////////////////////////////
// Some colors, for ease of use
////////////////////////////////////////////////////////////////////////////

#define COLOR_WHITE         0xF0F0F0
#define COLOR_ORANGE        0xF2B233
#define COLOR_MAGENTA       0xE57FD8
#define COLOR_LIGHT_BLUE    0x99B2F2
#define COLOR_YELLOW        0xDEDE6C
#define COLOR_LIME          0x7FCC19
#define COLOR_PINK          0xF2B2CC
#define COLOR_GRAY          0x4C4C4C
#define COLOR_LIGHT_GRAY    0x999999
#define COLOR_CYAN          0x4C99B2
#define COLOR_PURPLE        0xB266E5
#define COLOR_BLUE          0x3366CC
#define COLOR_BROWN         0x7F664C
#define COLOR_GREEN         0x57A64E
#define COLOR_RED           0xCC4C4C
#define COLOR_BLACK         0x191919

////////////////////////////////////////////////////////////////////////////
// Terminal functions
////////////////////////////////////////////////////////////////////////////

/**
 * Will initializr the terminal
 */
void term_init(multiboot_info_t* multiboot);

/**
 * Will write text to the terminal
 * 
 * TODO: Add support for formating like in printf
 */
void term_write(const char* text);

/**
 * Clear the terminal
 */
void term_clear();

/**
 * Set the text color
 */
void term_set_text_color(uint32_t color);

/**
 * Set the background color
 */
void term_set_background_color(uint32_t color);

/**
 * Get the text color
 */
uint32_t term_get_text_color();

/**
 * Get the background color
 */
uint32_t term_get_background_color();

/**
 * Set the cursor position
 */
void term_set_cursor_pos(uint32_t x, uint32_t y);

/**
 * Get the X position of the cursor
 */
uint32_t term_get_cursor_x();

/**
 * Get the Y position of the cursor
 */
uint32_t term_get_cursor_y();

/**
 * Get the terminal width, in characters
 */
uint32_t term_get_width();

/**
 * Get the terminal height, in characters
 */
uint32_t term_get_height();

/**
 * Scroll n lines down
 */
void term_scroll(uint32_t n);

/**
 * Clear line number n
 */
void term_clear_line(uint32_t n);

#endif 