#include <compo/screen/screen.h>
#include <proc/process.h>
#include <string.h>
#include "framebuffer.h"

typedef struct {
    struct screen screen;

    uint32_t* framebuffer;
    uint64_t pitch;
} framebuffer_t;

err_t framebuffer_blit(framebuffer_t* screen, uint32_t* buffer, size_t x, size_t y, size_t width, size_t height) {
    err_t err = NO_ERROR;

    // the pitch is assumed to be the width
    // TODO: allow that to be something different
    size_t pitch = width;

    // outside of sigh, no need to do anything
    if (x >= screen->screen.width) goto cleanup;
    if (y >= screen->screen.height) goto cleanup;

    // trim the width/height accordingly
    if (x + width >= screen->screen.width) width = screen->screen.width - x - 1;
    if (y + height >= screen->screen.height) height = screen->screen.height - y - 1;

    // do it
    for (int by = 0; by < height; by++) {
        // TODO: some sse memcpy because we are in a thread?
        //       or maybe that will fuck up the context of the thread...
        memcpy(&screen->framebuffer[(y + by) * screen->pitch + x], &buffer[by * pitch], sizeof(uint32_t) * width);
    }

cleanup:
    return err;
}

static framebuffer_t m_fb_screen = {
    .screen = {
        .component = {
                .type = COMPONENT_SCREEN,
                .ref_count = 1
        },
        .blit = (void*)framebuffer_blit
    }
};

void init_framebuffer_screen(tboot_info_t* info) {
    // generate the device address
    generate_device_address("framebuffer", sizeof("framebuffer") - 1, &m_fb_screen.screen.component.address);

    // map the framebuffer
    vmm_map(&kernel_process.vmm_handle,
            info->framebuffer.addr, PHYSICAL_TO_DIRECT(info->framebuffer.addr),
            info->framebuffer.height * info->framebuffer.pitch * 4,
            PAGE_SUPERVISOR_READWRITE, DEFAULT_CACHE);

    // setup the device info
    m_fb_screen.framebuffer = (uint32_t*)PHYSICAL_TO_DIRECT(info->framebuffer.addr);
    m_fb_screen.screen.width = info->framebuffer.width;
    m_fb_screen.screen.height = info->framebuffer.height;
    m_fb_screen.pitch = info->framebuffer.pitch;

    // register device
    register_component((component_t*)&m_fb_screen);
}


