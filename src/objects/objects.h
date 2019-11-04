#ifndef TOMATKERNEL_OBJECTS_H
#define TOMATKERNEL_OBJECTS_H

#include <memory/vmm.h>
#include <util/list.h>

typedef int handle_t;

typedef enum object_type {
    OBJECT_PROCESS,
    OBJECT_THREAD,
} object_type_t;

typedef struct object {
    /*
     * The handle of the object
     */
    handle_t handle;

    /*
     * The object type
     */
    object_type_t type;

    /*
     * The functions of this object
     */
    void* functions;

    /*
     * The usermode context
     * someone will get
     */
    size_t usermode_length;
    void* usermode;

    /*
     * Object list link
     */
    list_entry_t link;
} object_t;

handle_t generate_object_handle();

#endif //TOMATKERNEL_OBJECTS_H
