#include "term.h"
#include "text.h"
#include "graphics.h"

static logger_t logger = {0};

term_init_f term_init = NULL;
term_write_f term_write = NULL;
term_clear_f term_clear = NULL;
term_scroll_f term_scroll = NULL;
term_set_background_color_f term_set_background_color = NULL;
term_set_foreground_color_f term_set_foreground_color = NULL;
term_get_background_color_f term_get_background_color = NULL;
term_get_foreground_color_f term_get_foreground_color = NULL;

static void stub_term_set_background_color(uint32_t col) {
    (void)col;
}

static void stub_term_set_foreground_color(uint32_t col) {
    (void)col;
}

static uint32_t stub_term_get_background_color() {
    return 0;
}

static uint32_t stub_term_get_foreground_color() {
    return 0;
}

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

void term_early_init(multiboot_info_t* info) {
    switch(info->framebuffer.type) {
        case MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT: {
            text_early_init(info);

            term_init = text_init;
            term_write = text_write;
            term_clear = text_clear;
            term_scroll = text_scroll;

            // TODO: Add color support to text mode terminal
            term_set_background_color = stub_term_set_background_color;
            term_set_foreground_color = stub_term_set_foreground_color;
            term_get_background_color = stub_term_get_background_color;
            term_get_foreground_color = stub_term_get_foreground_color;

        } break;

        case MULTIBOOT_FRAMEBUFFER_TYPE_RGB: {
            graphics_early_init(info);

            term_init = graphics_init;
            term_write = graphics_write;
            term_clear = graphics_clear;
            term_scroll = graphics_scroll;
            term_set_background_color = graphics_set_background_color;
            term_set_foreground_color = graphics_set_foreground_color;
            term_get_background_color = graphics_get_background_color;
            term_get_foreground_color = graphics_get_foreground_color;

        } break;
        case MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED: {
            log_warn("Indexed framebuffer is not supported");
        } break;
        default: {
            log_warn("Unknown framebuffer type");
        } break;
    }

    if(term_write) {
        logger.write = term_write;
        logger.set_text_color = logger_set_text_color,
        logger.set_background_color = logger_set_background_color,
        logger_register(&logger);
    }
}
