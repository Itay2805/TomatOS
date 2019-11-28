#include <memory/vmm.h>
#include <tboot.h>
#include <memory/mm.h>
#include <util/defs.h>
#include <string.h>
#include "terminal.h"
#include "font.h"

static size_t fb_width = 0;
static size_t fb_height = 0;
static size_t fb_pitch = 0;

static uint32_t* framebuffer = NULL;
static uint32_t* backbuffer = NULL;

static size_t cursorX = 0;
static size_t cursorY = 0;

static size_t term_width = 0;
static size_t term_height = 0;

static uint32_t fg_color = 0xFFFFFF;
static uint32_t bg_color = 0x000000;

static bool is_inited = false;

void terminal_early_init(tboot_info_t* info) {
    fb_width = info->framebuffer.width;
    fb_height = info->framebuffer.height;
    fb_pitch = info->framebuffer.pitch;

    term_width = ALIGN_DOWN(fb_width, 8) / 8;
    term_height = ALIGN_DOWN(fb_height, 8) / 8;

    // will be the same for now
    framebuffer = (uint32_t*)info->framebuffer.addr;
    backbuffer = (uint32_t*)info->framebuffer.addr;

    // clear the screen
    for(size_t i = 0; i < term_height * 8; i++) {
        memset(framebuffer + i * fb_pitch, 0, 4 * fb_width);
        memset(backbuffer + i * fb_pitch, 0, 4 * fb_width);
    }

    is_inited = true;
}

void terminal_disable() {
    is_inited = false;
}

void terminal_init(tboot_info_t* info) {
    framebuffer = PHYSICAL_TO_DIRECT((uint32_t*)info->framebuffer.addr);
    backbuffer = mm_allocate_pages(SIZE_TO_PAGES(fb_width * fb_height * 4));

    // unmap if already mapped
    // this is so we can make sure it is mapped as wc
    if(vmm_is_mapped(&kernel_handle, (uintptr_t)framebuffer, fb_pitch * fb_height * 4)) {
        vmm_unmap(&kernel_handle, (uintptr_t)framebuffer, fb_pitch * fb_height * 4);
    }

    // map the framebuffer
    vmm_map(&kernel_handle, info->framebuffer.addr, (uintptr_t)framebuffer, fb_pitch * fb_height * 4, PAGE_SUPERVISOR_READWRITE, IA32_PAT_MEMTYPE_WC);

    for(size_t i = 0; i < term_height * 8; i++) {
        memcpy(backbuffer + i * fb_pitch, framebuffer + i * fb_pitch, 4 * fb_width);
    }

    is_inited = true;
}

static void draw_char(const char* bitmap) {
    size_t base_x = cursorX * 8;
    size_t base_y = cursorY * 8;

    for (size_t y = base_y; y < base_y + 8; y++) {
        for (size_t x = base_x; x < base_x + 8; x++) {
            uint32_t color;
            if(bitmap[y - base_y] & (1u << (x - base_x))) {
                color = fg_color;
            }else {
                color = bg_color;
            }

            framebuffer[x + y * fb_pitch] = color;
            if(likely(backbuffer != framebuffer)) {
                backbuffer[x + y * fb_width] = color;
            }
        }
    }

    cursorX++;
}

static void scroll() {
    for(size_t i = 8; i < term_height * 8; i++) {
        memcpy(framebuffer + (i - 8) * fb_pitch, backbuffer + i * fb_width, 4 * fb_width);

        if(likely(backbuffer != framebuffer)) {
            memcpy(backbuffer + (i - 8) * fb_width, backbuffer + i * fb_width, 4 * fb_width);
        }
    }
    for(size_t i = ((term_height - 1) * 8); i < term_height * 8; i++) {
        memset(framebuffer + i * fb_pitch, 0, 4 * fb_width);
        if(likely(backbuffer != framebuffer)) {
            memset(backbuffer + i * fb_pitch, 0, 4 * fb_width);
        }
    }
}

void terminal_write_char(char c) {
    if(!is_inited) {
        return;
    }

    if(c == '\t') {
        terminal_write_char(' ');
        terminal_write_char(' ');
    }else if(c == '\n') {
        cursorY++;
        cursorX = 0;
    }else {
        draw_char(font_basic[(int)c]);
        if(cursorX >= term_width) {
            cursorX = 0;
            cursorY++;
        }
    }

    if(cursorY >= term_height) {
        cursorY = term_height - 1;
        cursorX = 0;
        scroll();
    }
}