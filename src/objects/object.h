#ifndef TOMATKERNEL_RIVER_H
#define TOMATKERNEL_RIVER_H

#include <common/error.h>

typedef enum trait_type {
    TRAIT_SCREEN,
    TRAIT_COUNT,
} trait_type_t;

typedef int obj_id_t;

typedef struct trait {
    bool available;
    // array of syscalls
    size_t* syscalls;
    // array of functions
    uintptr_t* functions;
} trait_t;

typedef struct object {
    // unique object id
    obj_id_t id;

    // context
    void* context;

    // array of traits
    trait_t traits[TRAIT_COUNT];
} object_t;


error_t object_add(object_t* object);

error_t object_get_primary(trait_type_t trait, object_t** object);

error_t object_set_primary(trait_type_t trait, object_t* object);

#endif //TOMATKERNEL_RIVER_H
