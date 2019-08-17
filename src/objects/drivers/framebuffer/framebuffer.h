#ifndef TOMATKERNEL_FRAMEBUFFER_H
#define TOMATKERNEL_FRAMEBUFFER_H

#include <resources/object.h>
#include <tboot/tboot.h>

/**
 * Will initialize and register the framebuffer
 * as a screen object
 */
error_t framebuffer_init(tboot_info_t* info);

#endif //TOMATKERNEL_FRAMEBUFFER_H
