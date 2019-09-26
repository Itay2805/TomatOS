#include "term.h"
#include <common.h>
#include <tboot/tboot.h>
#include <libc/stdlib.h>
#include <memory/vmm.h>
#include <string.h>

#include "font/gfxfont.h"
#include "font/font_DejaVuSansMono.h"

////////////////////////////////////////////////
// The global state
////////////////////////////////////////////////

static uint32_t* vram;
static uint32_t* backbuffer;
static int width;
static int height;
static int full_height;
static uint32_t cur_x = 0, cur_y = 0;
static uint32_t fg_color = 0xFFFFFF, bg_color = 0x000000;
static const GFXfont* const font = &DejaVuSansMono8pt7b;

////////////////////////////////////////////////
// Logger stubs
////////////////////////////////////////////////

static logger_t logger = {0};

static uint32_t logger_set_text_color(uint32_t col) {
    uint32_t old = term_get_foreground_color();
    term_set_foreground_color(col);
    return old;
}

static uint32_t logger_set_background_color(uint32_t col) {
    uint32_t old = term_get_background_color();
    term_set_background_color(col);
    return old;
}

////////////////////////////////////////////////
// Init
////////////////////////////////////////////////

void term_early_init(tboot_info_t* info) {
    // set everything
    vram = (uint32_t*)info->framebuffer.addr;
    width = info->framebuffer.width;
    full_height = info->framebuffer.height;
    height = full_height - full_height % font->yAdvance;
    backbuffer = vram;

    // register logger
    logger.write = term_write;
    logger.set_text_color = logger_set_text_color,
    logger.set_background_color = logger_set_background_color,
    logger_register(&logger);
}

void term_init() {
    // map the framebuffer
    vmm_map_direct((uintptr_t) vram, width * full_height * 4u, (page_attrs_t){ .write = true, .global = true, .caching = PAGE_CACHE_WRITE_COMBINING });
    vram = CONVERT_TO_DIRECT(vram);

    // allocate a proper backbuffer
    uint32_t* tmp_backbuffer = vmalloc(width * full_height * 4u);
    memcpy(tmp_backbuffer, vram, width * full_height * 4u);
    backbuffer = tmp_backbuffer;
}

////////////////////////////////////////////////
// Rendering
////////////////////////////////////////////////

static void render_characer(const GFXglyph* const glyph, uint8_t* bitmap)  {
    uint16_t segmentIndex = glyph->bitmapOffset;
    uint8_t segment = 0;
    uint8_t bit_index = 0;
    for(uint8_t y_index = 0; y_index < glyph->height; y_index++) {
        for(uint8_t x_index = 0; x_index < glyph->width; x_index++) {
            if(!(bit_index++ & 7)) {
                segment = bitmap[segmentIndex++];
            }
            uint32_t screen_x = cur_x + glyph->xOffset + x_index;
            uint32_t screen_y = (cur_y + font->yAdvance) + glyph->yOffset + y_index;

            // don't write outside of buffer
            if(screen_x + screen_y * width >= width * full_height) continue;

            // actual writing to buffer
            if(segment & 0x80) {
                backbuffer[screen_x + screen_y * width] = fg_color;
                vram[screen_x + screen_y * width] = fg_color;
            }else {
                backbuffer[screen_x + screen_y * width] = bg_color;
                vram[screen_x + screen_y * width] = bg_color;
            }

            segment <<= 1;
        }
    }
}

static void scroll() {
    for(size_t i = 0; i < (full_height * width) - (font->yAdvance * width); i++) {
        uint32_t col = backbuffer[i + (font->yAdvance * width)];
        backbuffer[i] = col;
        vram[i] = col;
    }
    for(int i = (full_height * width) - (font->yAdvance * width); i <  width * full_height; i++) {
        backbuffer[i] = bg_color;
        vram[i] = bg_color;
    }
}

static void new_line() {
    cur_x = 0;
    cur_y += font->yAdvance;
    while(cur_y >= height) {
        cur_x = 0;
        cur_y -= font->yAdvance;
        scroll();
    }
}

////////////////////////////////////////////////
// Terminal API functions
////////////////////////////////////////////////

void term_write(const char* str) {
    while(*str) {
        if(*str >= font->first && *str <= font->last) {
            const GFXglyph* const glyph = &font->glyph[*str - font->first];

            // this will overflow from the screen, prevent
            if(cur_x + glyph->xAdvance >= width) {
                new_line();
            }

            // draw the character
            if(glyph->width && glyph->height) {
                render_characer(glyph, font->bitmap);
            }

            // move the needed amount
            cur_x += glyph->xAdvance;
        }else if(*str == '\n') {
            // got a new line!
            new_line();
        }else if(*str == '\t') {
            term_write("  ");
        }

        str++;
    }
}

void term_clear() {
    for(int i = 0; i < width * height; i++) {
        vram[i] = bg_color;
    }
}

void term_scroll() {
    scroll();
}

void term_set_background_color(uint32_t col) {
    bg_color = col;
}

void term_set_foreground_color(uint32_t col) {
    fg_color = col;
}

uint32_t term_get_background_color() {
    return bg_color;
}

uint32_t term_get_foreground_color() {
    return fg_color;
}

void term_disable() {
    logger.enabled = false;
}