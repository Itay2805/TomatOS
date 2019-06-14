#include "graphics.h"
#include "font.h"
#include "term.h"

#include <memory/vmm.h>
#include <stdbool.h>
#include <boot/multiboot.h>
#include <common.h>

static uint32_t* vram;
static int cols;
static int rows;
static int width;
static int height;
static int cur_x = 0, cur_y = 0;
static uint32_t bg_color = 0x000000, fg_color = 0xD3D7CF;
static bool enabled = false;

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
                vram[scrn_x + scrn_y * width] = fg_color;
            }else {
                vram[scrn_x + scrn_y * width] = bg_color;
            }
        }
    }
}

void graphics_early_init(multiboot_info_t* info) {
    vram = (uint32_t*)info->framebuffer.addr;

    width = info->framebuffer.width;
    height = info->framebuffer.height;

    cols = info->framebuffer.width / 8;
    rows = info->framebuffer.height / 8;
}

void graphics_init() {
    for(uint64_t addr = ALIGN_DOWN(vram, KB(4)); addr < ALIGN_UP((uint64_t)vram + width * height * 32, KB(4)); addr += KB(4)) {
        if(!vmm_is_mapped(kernel_address_space, (uintptr_t) PHYSICAL_ADDRESS(addr))) {
            vmm_map(kernel_address_space, PHYSICAL_ADDRESS(addr), (void *) addr, PAGE_ATTR_WRITE);
        }
    }
    vram = PHYSICAL_ADDRESS(vram);
    graphics_clear();
    enabled = true;
}

void graphics_write(const char* text) {
    if(!enabled) return;
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
        if(cur_x >= cols) {
            cur_x = 0;
            cur_y++;
        }
        if(cur_y >= rows) {
            term_scroll(1);
            cur_y = rows - 1;
        }
    }
}

void graphics_clear() {
    if(!enabled) return;
    for(int i = 0; i < width * height; i++) {
        vram[i] = bg_color;
    }
}

void graphics_scroll(uint16_t n) {
    if(!enabled) return;
    n *= 8;
    for(size_t i = 0; i <  (height - n) * width; i++) {
        vram[i] = vram[i + (n * height)];
    }
    for(int i = (height - n) * width; i <  width * height; i++) {
        vram[i] = bg_color;
    }
}
