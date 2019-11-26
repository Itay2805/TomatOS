#ifndef TOMATKERNEL_DEVICE_H
#define TOMATKERNEL_DEVICE_H

#include <util/list.h>

typedef enum device_type {
    DEVICE_STORAGE,
    DEVICE_PARTITION,
    DEVICE_FILESYSTEM,
    DEVICE_FILE,
} device_type_t;

typedef struct device {
    // the device type
    device_type_t type;

    // device names
    // TODO: should these be unique
    char name[255];

    // link to the device list (of this type)
    list_entry_t link;
} device_t;

#endif //TOMATKERNEL_DEVICE_H
