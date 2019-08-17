#include <tboot/tboot.h>
#include "framebuffer.h"

#include <resources/screen.h>
#include <string.h>

static uintptr_t base;
static size_t width;
static size_t height;

static error_t clear(object_t* obj) {
    // memset((void *) base, 0, (size_t) (width * height * 4));
    log_info("Called screen clear!");
    return NO_ERROR;
}

static uintptr_t functions[] = {
    [SCREEN_FUNCTION_CLEAR] = (uintptr_t)clear
};

static uintptr_t syscalls[SCREEN_FUNCTION_COUNT] = {};

static object_t obj = {
    .traits = {
        [TRAIT_SCREEN] = (trait_t){
            .available = true,
            .functions = functions,
            .syscalls = syscalls,
        },
    }
};

error_t framebuffer_init(tboot_info_t* info) {
    error_t err = NO_ERROR;
    base = info->framebuffer.addr;
    width = info->framebuffer.width;
    height = info->framebuffer.height;

    // TODO: syscall allocation

    CHECK_AND_RETHROW(object_add(&obj));

cleanup:
    return err;
}


