#include <stb/stb_ds.h>
#include <common/locks/spinlock.h>
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
    [OBJECT_DISPLAY] = "Display",
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
