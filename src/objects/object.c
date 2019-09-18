#include <stb/stb_ds.h>
#include <common/locks/spinlock.h>
#include <processes/syscalls.h>
#include <processes/syscall.h>
#include "object.h"

static spinlock_t objects_lock;
static struct {
    obj_id_t key;
    object_t* value;
}* objects;

static int id = 0;
static spinlock_t primary_objects_lock;
static object_t* primary_objects[OBJECT_TYPE_MAX];

static const char* type_names[] = {
    [OBJECT_STORAGE] = "Storage",
    [OBJECT_PARTITION] = "Partition",
    [OBJECT_FILESYSTEM] = "Filesystem",
    [OBJECT_NETWORK] = "Network",
};

error_t object_add(object_t* object) {
    error_t err = NO_ERROR;

    CHECK(object != NULL);

    lock(&objects_lock);
    object->id = ++id;
    hmput(objects, object->id, object);
    unlock(&objects_lock);

    log_info("Added object %s (#%d)", object->name, object->id);

    lock(&primary_objects_lock);
    bool has_primary = primary_objects[object->type] == NULL;
    unlock(&primary_objects_lock);

    if(has_primary) {
        CHECK_AND_RETHROW(object_set_primary(object));
    }

cleanup:
    return err;
}

error_t object_get_primary(object_type_t type, object_t** object) {
    error_t err = NO_ERROR;

    CHECK(object != NULL);

    lock(&primary_objects_lock);
    *object = primary_objects[type];
    unlock(&primary_objects_lock);

cleanup:
    return err;
}

error_t object_set_primary(object_t* object) {
    error_t err = NO_ERROR;

    CHECK(object != NULL);

    lock(&primary_objects_lock);
    log_info("object %s (#%d) is now primary object for %s type", object->name, object->id, type_names[object->type]);
    primary_objects[object->type] = object;
    unlock(&primary_objects_lock);

cleanup:
    return err;
}

/***************************************************************
 * Syscalls related stuff
 ***************************************************************/

static error_t object_get_primary_syscall(syscall_context_t* context) {
    error_t err = NO_ERROR;

    object_type_t type = (object_type_t) (object_type_t*)SYSARG0(context);
    uint64_t* buffer = (uint64_t *) SYSARG1(context);
    size_t size = SYSARG2(context);

    // verify input
    CHECK(IS_USER_POINTER((uintptr_t)buffer + size));
    CHECK(type <= OBJECT_TYPE_MAX);

    // do it
    object_t* obj = NULL;
    CHECK_AND_RETHROW(object_get_primary(type, &obj));

    // get the element count
    size /= sizeof(uintptr_t);
    CHECK(size > 0);

    // set the id
    buffer[0] = (uint64_t) obj->id;

    // set the rest
    if(size > obj->syscall_count) {
        size = obj->syscall_count;
    }

    // copy the functions
    memcpy(buffer, obj->syscalls, size * sizeof(uintptr_t));

cleanup:
    return err;
}

error_t object_init_syscalls() {
    error_t err = NO_ERROR;

    CHECK_AND_RETHROW(syscall_register(object_get_primary_syscall, SYSCALL_OBJECT_GET_PRIMARY));

cleanup:
    return err;
}

