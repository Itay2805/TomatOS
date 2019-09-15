#include "framebuffer.h"

#include <objects/display.h>
#include <tboot/tboot.h>
#include <string.h>

static uintptr_t base;
static size_t width;
static size_t height;

static error_t get_framebuffer(uintptr_t* framebuffer, size_t* out_width, size_t* out_height) {
    error_t err = NO_ERROR;

    CHECK(framebuffer != NULL);
    CHECK(out_width != NULL);
    CHECK(out_height != NULL);

    *framebuffer = base;
    *out_width = width;
    *out_height = height;

cleanup:
    return err;
}

static uintptr_t functions[] = {
    [DISPLAY_SYS_GET_FRAMEBUFFER] = (uintptr_t)get_framebuffer
};

static uintptr_t syscalls[DISPLAY_SYS_MAX] = {};

static object_t obj = {
    .type = OBJECT_DISPLAY,
    .name = "framebuffer",
    .functions = functions,
    .syscalls = syscalls
};

error_t framebuffer_init(tboot_info_t* info) {
    error_t err = NO_ERROR;

    CHECK(info != NULL);

    base = info->framebuffer.addr;
    width = info->framebuffer.width;
    height = info->framebuffer.height;

    // TODO: syscall allocation

    CHECK_AND_RETHROW(object_add(&obj));

cleanup:
    return err;
}


