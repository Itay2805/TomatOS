#ifndef TOMATKERNEL_SCREEN_H
#define TOMATKERNEL_SCREEN_H

#include <common/error.h>
#include "object.h"

typedef enum display_syscalls {
    DISPLAY_SYS_GET_FRAMEBUFFER,

    DISPLAY_SYS_MAX
} display_syscalls_t;

typedef struct display_functions {
    error_t (*get_framebuffer)(uintptr_t* framebuffer, size_t* width, size_t* height);
} display_functions_t;

#endif //TOMATKERNEL_SCREEN_H
