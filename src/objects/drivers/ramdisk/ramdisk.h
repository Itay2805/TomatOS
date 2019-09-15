#ifndef TOMATKERNEL_RAMDISK_H
#define TOMATKERNEL_RAMDISK_H

#include <common/error.h>

typedef struct ramdisk {
    void** sectors;
} ramdisk_t;

/**
 * Create a new ramdisk
 *
 * @remark
 * Size must be 4k aligned!
 *
 * @param size  [IN] Ramdisk size
 */
error_t ramdisk_create(size_t size);

#endif //TOMATKERNEL_RAMDISK_H
