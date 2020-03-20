#ifndef __COMPO_SCREEN_WINDOW_H__
#define __COMPO_SCREEN_WINDOW_H__

#include <stdint.h>
#include <event/event.h>

typedef struct window_data {
    // this is used for passing events to the process
    event_t event;

    // this is the framebuffer of the window
    void* framebuffer;
} window_data_t;

typedef window_data_t* window_t;

#endif //__COMPO_SCREEN_WINDOW_H__
