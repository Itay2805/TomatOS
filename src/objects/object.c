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
static object_t* primary_objects[TRAIT_COUNT];

static const char* trait_names[] = {
    [TRAIT_SCREEN] = "Screen",
};

error_t object_add(object_t* object) {
    lock(&objects_lock);
    object->id = ++id;
    hmput(objects, object->id, object);
    unlock(&objects_lock);

    log_info("Added objects #%d", object->id);

    lock(&primary_objects_lock);
    for(int i = 0; i < TRAIT_COUNT; i++) {
        if(object->traits[i].available && primary_objects[i] == NULL) {
            log_info("object #%d is now primary object for %s trait", object->id, trait_names[i]);
            primary_objects[i] = object;
        }
    }
    unlock(&primary_objects_lock);
    return NO_ERROR;
}

error_t object_get_primary(trait_type_t trait, object_t** object) {
    lock(&primary_objects_lock);
    *object = primary_objects[trait];
    unlock(&primary_objects_lock);
    return NO_ERROR;
}

error_t object_set_primary(trait_type_t trait, object_t* object) {
    lock(&primary_objects_lock);
    log_info("object #%d is now primary object for %s trait", object->id, trait_names[trait]);
    primary_objects[trait] = object;
    unlock(&primary_objects_lock);
    return NO_ERROR;
}
