#include "graphics.h"
#include "font.h"
#include "term.h"

#include <locks/spinlock.h>
#include <memory/vmm.h>
#include <stdbool.h>
#include <common.h>
#include <stdlib.h>
#include <string.h>

static uint32_t* vram;
static uint32_t* backbuffer;
static int width;
static int height;
static int cur_x = 0, cur_y = 0;
static uint32_t bg_color = 0x000000, fg_color = 0xD3D7CF;
static spinlock_t gfx_lock;

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
                backbuffer[scrn_x + scrn_y * width * 8] = fg_color;
                vram[scrn_x + scrn_y * width * 8] = fg_color;
            }else {
                backbuffer[scrn_x + scrn_y * width * 8] = bg_color;
                vram[scrn_x + scrn_y * width * 8] = bg_color;
            }
        }
    }
}

/**
 * The main reason we have this is because this will not lock,
 * while the other scroll will lock
 */
static void scroll_internal(uint16_t n) {
    for(size_t i = 0; i <  (height - n) * width * 8 * 8; i++) {
        uint32_t col = backbuffer[i + (n * width * 8 * 8)];
        backbuffer[i] = col;
        vram[i] = col;
    }
    for(int i = (height - n) * width * 8 * 8; i <  width * height * 8 * 8; i++) {
        backbuffer[i] = bg_color;
        vram[i] = bg_color;
    }
}

void graphics_init(boot_info_t* info) {
    vram = (uint32_t*)info->framebuffer.addr;
    width = (int) (info->framebuffer.width / 8);
    height = (int) (info->framebuffer.height / 8);
    for(uint64_t addr = ALIGN_DOWN(vram, KB(4)); addr < ALIGN_UP((uint64_t)vram + width * height * 8 * 8 * 4, KB(4)); addr += KB(4)) {
        if(!vmm_is_mapped(kernel_address_space, (uintptr_t) addr)) {
            vmm_map(kernel_address_space, (void *) addr, (void *) addr - PHYSICAL_BASE, PAGE_ATTR_WRITE);
        }
    }
    graphics_clear();
    backbuffer = malloc((size_t) (width * height * 8 * 8 * 4));
}

void graphics_write(const char* text) {
    lock_preemption(&gfx_lock);
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

    unlock_preemption(&gfx_lock);
}

void graphics_clear() {
    for(int i = 0; i < width * height; i++) {
        vram[i] = bg_color;
    }
}

void graphics_scroll(uint16_t n) {
    lock_preemption(&gfx_lock);
    scroll_internal(n);
    unlock_preemption(&gfx_lock);
}

void graphics_set_background_color(uint32_t col) {
    lock_preemption(&gfx_lock);
    bg_color = col;
    unlock_preemption(&gfx_lock);
}

void graphics_set_foreground_color(uint32_t col) {
    lock_preemption(&gfx_lock);
    fg_color = col;
    unlock_preemption(&gfx_lock);
}

uint32_t graphics_get_background_color() {
    return bg_color;
}

uint32_t graphics_get_foreground_color() {
    return fg_color;
}