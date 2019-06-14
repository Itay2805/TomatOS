#include "term.h"
#include "text.h"

static logger_t logger = {0};

term_init_f term_init = NULL;
term_write_f term_write = NULL;
term_clear_f term_clear = NULL;
term_scroll_f term_scroll = NULL;

void term_early_init(multiboot_info_t* info) {
    switch(info->framebuffer.type) {
        case MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT: {
            text_early_init(info);

            term_init = text_init;
            term_write = text_write;
            term_clear = text_clear;
            term_scroll = text_scroll;

        } break;

        case MULTIBOOT_FRAMEBUFFER_TYPE_RGB: {
            log_warn("RGB framebuffer is not supported yet");
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
        logger_register(&logger);
    }
}
