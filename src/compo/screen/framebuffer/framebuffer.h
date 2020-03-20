#ifndef __COMPO_SCREEN_FRAMEBUFFER_H__
#define __COMPO_SCREEN_FRAMEBUFFER_H__

#include <util/except.h>

/**
 * Create a framebuffer screen from the framebuffer provided
 * by the bootloader
 *
 * @param info  [IN] The bootloader information
 */
void init_framebuffer_screen(tboot_info_t* info);

#endif //__COMPO_SCREEN_FRAMEBUFFER_H__
