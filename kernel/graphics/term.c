#include <common/mini-printf.h>
#include "term.h"

#include "font.h"

static uint32_t* vram;
static int width;
static int height;
static int cur_x = 0, cur_y = 0;
static uint32_t bg_color = COLOR_BLACK, fg_color = COLOR_WHITE;

/**
 * Used to draw a single character
 *
 * TODO: Maybe add some scaling to make it easier to view
 */
static void draw_char(int chr) {
    char* letter;

    if(chr < 0 || chr >= 128) {
        return;
    }

    letter = font_basic[chr];

    for(int y = 0; y < 8; y++) {
        for(int x = 0; x < 8; x++) {
            int scrn_y = y + cur_y * 8;
            int scrn_x = x + cur_x * 8;
            int index = x + y * 8;
            if((letter[index / 8] & (1 << index % 8)) != 0) {
                vram[scrn_x + scrn_y * width * 8] = fg_color;
            }else {
                vram[scrn_x + scrn_y * width * 8] = bg_color;
            }
        }
    }
}

void term_init(multiboot_info_t* multiboot) {
    vram = (uint32_t*)multiboot->framebuffer_addr;

    width = multiboot->framebuffer_width / 8;
    height = multiboot->framebuffer_height / 8;

    // TODO: Right now we are assuming 32bpp, should have it more dynamic

    term_clear();
    term_print("[term_init] width=%d, height=%d\n", width, height);
}

void term_write(const char* text) {
    while(*text) {
        char chr = *text++;
        switch(chr) {
            case '\n':
                cur_x = 0;
                cur_y++;
                break;
            case ' ':
            case '\t':
                draw_char(' ');
                cur_x++;
                break;
            default:
                draw_char(chr);
                cur_x++;
                break;
        }
        if(cur_x >= width) {
            cur_x = 0;
            cur_y++;
        }
        if(cur_y >= height) {
            term_scroll(1);
            cur_y = height - 1;
        }
    }
}

size_t term_print(const char* fmt, ...) {
    va_list list;
    va_start(list, fmt);
    char buffer[1024];
    int printed = mini_vsnprintf(buffer, 1024u, fmt, list);
    term_write(buffer);
    va_end(list);
    return (size_t) printed;
}

void term_clear() {
    for(int i = 0; i < width * height * 8 * 8; i++) {
        vram[i] = bg_color;
    }
}

void term_set_text_color(uint32_t color) {
    fg_color = color;
}

void term_set_background_color(uint32_t color) {
    bg_color = color;
}

uint32_t term_get_text_color() {
    return fg_color;
}

uint32_t term_get_background_color() {
    return bg_color;
}

void term_set_cursor_pos(uint32_t x, uint32_t y) {
    // TODO: Bound checking
    cur_x = x;
    cur_y = y;
}

uint32_t term_get_cursor_x() {
    return cur_x;
}

uint32_t term_get_cursor_y() {
    return cur_y;
}

uint32_t term_get_width() {
    return width;
}

uint32_t term_get_height() {
    return height;
}

void term_scroll(uint32_t n) {
    for(size_t i = 0; i <  (height - n) * width * 8 * 8; i++) {
        vram[i] = vram[i + (n * width * 8 * 8)];
    }
    for(int i = (height - n) * width * 8 * 8; i <  width * height * 8 * 8; i++) {
        vram[i] = bg_color;
    }
}

void term_clear_line(uint32_t n) {
    for(size_t y = n; y < n + 8; y++) {
        for(int x = 0; x < width * 8; x++) {
            vram[x + y * width] = bg_color;
        }
    }
}
