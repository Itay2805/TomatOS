#ifndef TOMATKERNEL_RIVER_H
#define TOMATKERNEL_RIVER_H

#include <common/error.h>

typedef enum object_type {
    OBJECT_STORAGE,
    OBJECT_PARTITION,
    OBJECT_FILESYSTEM,
    OBJECT_NETWORK,
    OBJECT_TYPE_MAX
} object_type_t;

typedef int obj_id_t;

typedef struct object {
    obj_id_t id;
    const char* name;
    void* context;
    object_type_t type;

    // syscalls
    size_t syscall_count;
    size_t* syscalls;
    void* functions;
} object_t;

/**
 * Init the syscalls
 */
error_t object_init_syscalls();

/**
 * A new object to the object map
 *
 * @param object    [IN] The object to add
 */
error_t object_add(object_t* object);

error_t object_get_primary(object_type_t type, object_t** object);

error_t object_set_primary(object_t* object);

/**
 * Will get the object at the given index of the given type
 *
 * @param object    [OUT]   The pointer to the object
 * @param index     [IN]    The index to search
 * @param type      [IN]    The type to filter on
 */
//error_t object_get(object_t** object, int index, object_type_t type);

#endif //TOMATKERNEL_RIVER_H
