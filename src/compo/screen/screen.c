#include <tboot.h>
#include <mm/vmm.h>
#include <proc/process.h>

#include "screen.h"

err_t screen_blit(screen_t screen, void* buffer, size_t x, size_t y, size_t width, size_t height) {
    err_t err = NO_ERROR;

    CHECK_ERROR(screen != NULL, ERROR_INVALID_PARAM);
    CHECK_ERROR(buffer != NULL, ERROR_INVALID_PARAM);
    CHECK_ERROR(screen->component.type == COMPONENT_SCREEN, ERROR_INVALID_HANDLE);

    CHECK_AND_RETHROW(screen->blit(screen, buffer, x, y, width, height));

cleanup:
    return err;
}
