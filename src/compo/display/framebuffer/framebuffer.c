#include <compo/display/display.h>
#include <proc/process.h>
#include <string.h>
#include "framebuffer.h"

typedef struct {
    struct display display;

    uint32_t* framebuffer;
    uint64_t pitch;
} framebuffer_t;

err_t framebuffer_blit(framebuffer_t* screen, uint32_t* buffer, size_t x, size_t y, size_t width, size_t height) {
    err_t err = NO_ERROR;

    // the pitch is assumed to be the width
    // TODO: allow that to be something different
    size_t pitch = width;

    // outside of sigh, no need to do anything
    if (x >= screen->display.width) goto cleanup;
    if (y >= screen->display.height) goto cleanup;

    // trim the width/height accordingly
    if (x + width >= screen->display.width) width = screen->display.width - x - 1;
    if (y + height >= screen->display.height) height = screen->display.height - y - 1;

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
    .display = {
        .component = {
                .type = COMPONENT_DISPLAY,
                .ref_count = 1
        },
        .blit = (void*)framebuffer_blit
    }
};

void init_framebuffer_screen(stivale_struct_t* info) {
    // generate the device address
    generate_device_address("framebuffer", sizeof("framebuffer") - 1, &m_fb_screen.display.component.address);

    // map the framebuffer
    vmm_map(&kernel_process.vmm_handle,
            info->framebuffer_addr, PHYSICAL_TO_DIRECT(info->framebuffer_addr),
            info->framebuffer_height * info->framebuffer_pitch,
            PAGE_SUPERVISOR_READWRITE, DEFAULT_CACHE);

    // setup the device info
    m_fb_screen.framebuffer = (uint32_t*)PHYSICAL_TO_DIRECT(info->framebuffer_addr);
    m_fb_screen.display.width = info->framebuffer_width;
    m_fb_screen.display.height = info->framebuffer_height;
    m_fb_screen.pitch = info->framebuffer_pitch;

    // register device
    register_component((component_t*)&m_fb_screen);
}


