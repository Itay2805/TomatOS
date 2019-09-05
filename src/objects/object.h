#ifndef TOMATKERNEL_RIVER_H
#define TOMATKERNEL_RIVER_H

#include <common/error.h>

typedef enum object_type {
    OBJECT_STORAGE,
    OBJECT_PARTITION,
    OBJECT_FILESYSTEM,
    OBJECT_NETWORK,
    OBJECT_DISPLAY,
    OBJECT_TYPE_MAX
} object_type_t;

typedef int obj_id_t;

typedef struct object {
    obj_id_t id;
    void* context;
    object_type_t type;

    // syscalls
    size_t* syscalls;
    void* functions;
} object_t;

error_t object_add(object_t* object);

error_t object_get_primary(object_type_t type, object_t** object);

error_t object_set_primary(object_t* object);

#endif //TOMATKERNEL_RIVER_H
