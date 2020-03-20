#ifndef __COMPO_SCREEN_SCREEN_H__
#define __COMPO_SCREEN_SCREEN_H__

#include <tboot.h>
#include <compo/component.h>

typedef struct screen {
    component_t component;
    err_t (*blit)(struct screen*, void* buffer, size_t x, size_t y, size_t width, size_t height);
}* screen_t;

/**
 * Blit a buffer to the screen
 *
 * @remark
 * This will cut the buffer if it reaches out of the screen
 *
 * @param screen    [IN] The screen to blit to
 * @param buffer    [IN] The buffer we are going to blit
 * @param x         [IN] The x position
 * @param y         [IN] The y position
 * @param width     [IN] The width of the blit buffer
 * @param height    [IN] The height of the blit buffer
 *
 * //TODO: Pitch?
 */
err_t screen_blit(screen_t screen, void* buffer, size_t x, size_t y, size_t width, size_t height);

#endif //__COMPO_SCREEN_SCREEN_H__
