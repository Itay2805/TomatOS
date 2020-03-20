#include <crypto/sha1.h>
#include <sync/spinlock.h>
#include <string.h>
#include "component.h"

// list with specific components
// TODO: eh
static list_entry_t fs_components = INIT_LIST_ENTRY(fs_components);
static list_entry_t screen_components = INIT_LIST_ENTRY(screen_components);
static list_entry_t keyboard_components = INIT_LIST_ENTRY(keyboard_components);
static list_entry_t mouse_components = INIT_LIST_ENTRY(mouse_components);

// list with all components
static spinlock_t components_lock = SPINLOCK_INIT;
static list_entry_t all_components = INIT_LIST_ENTRY(all_components);

// the primary of each component type
static component_t* primary_components[COMPONENT_MAX_TYPE] = {0};

// component type to name map
static const char* type_to_name[] = {
    [COMPONENT_FILESYSTEM] = "Filesystem",
    [COMPONENT_SCREEN] = "Screen",
    [COMPONENT_MOUSE] = "Mouse",
    [COMPONENT_KEYBOARD] = "Keyboard"
};

void generate_device_address(void* address, size_t len, address_t* addr) {
    ASSERT(addr != NULL);
    ASSERT(address != NULL);
    ASSERT(len != 0);

    SHA1_CTX ctx;
    sha1_init(&ctx);
    sha1_update(&ctx, address, len);
    sha1_final(&ctx, (uint8_t*)addr);
}

void register_component(component_t* component) {
    ASSERT(component != NULL);

    // get the lock and stuff
    list_entry_t* list = NULL;
    switch (component->type) {
        case COMPONENT_FILESYSTEM:
            list = &fs_components;
            break;

        case COMPONENT_SCREEN:
            list = &screen_components;
            break;

        case COMPONENT_MOUSE:
            list = &mouse_components;
            break;

        case COMPONENT_KEYBOARD:
            list = &keyboard_components;
            break;

        default: ASSERT(!"Invalid component type");
    }

    // insert it
    spinlock_acquire(&components_lock);
    insert_tail_list(list, &component->type_link);
    insert_tail_list(&all_components, &component->link);
    spinlock_release(&components_lock);

    address_t* addr = &component->address;
    TRACE("Added %s {%08X-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX}",
          type_to_name[component->type],
          addr->data1, addr->data2, addr->data3,
          addr->data4[0], addr->data4[1], addr->data4[2], addr->data4[3],
          addr->data4[4], addr->data4[5], addr->data4[6], addr->data4[7]);
}

err_t get_component(address_t address, component_t** comp) {
    err_t err = NO_ERROR;

    CHECK_ERROR(comp != NULL, ERROR_INVALID_PARAM);

    spinlock_acquire(&components_lock);

    FOR_EACH_IN_LIST(all_components, link) {
        component_t* component = CR(link, component_t, link);
        if (component->ref_count != 0 && memcmp(&address, &component->address, 16) == 0) {
            component->ref_count++;
            *comp = component;
            goto cleanup;
        }
    }

    CHECK_FAIL_ERROR(ERROR_NOT_FOUND);

cleanup:
    spinlock_release(&components_lock);
    return err;
}

err_t release_component(void* comp) {
    err_t err = NO_ERROR;
    component_t* c = comp;

    CHECK_ERROR(c != NULL, ERROR_INVALID_PARAM);

    // decrefement the ref count and if zero free it
    if (c->ref_count-- == 0) {

        // remove the link
        remove_entry_list(&c->type_link);
        remove_entry_list(&c->link);

        // if has a deleted delete it
        if (c->delete != NULL) {
            CHECK_AND_RETHROW(c->delete(c));
        }
    }

cleanup:
    return err;
}

err_t get_next_component(component_type_t type, component_t** comp) {
    ASSERT(false);
}

err_t get_primary(component_type_t type, component_t** comp) {
    err_t err = NO_ERROR;

    spinlock_acquire(&components_lock);

    CHECK_ERROR(comp != NULL, ERROR_INVALID_PARAM);
    CHECK_ERROR(primary_components[type] != NULL, ERROR_NOT_FOUND);

    // increment the ref count and set it
    primary_components[type]->ref_count++;
    *comp = primary_components[type];


cleanup:
    spinlock_release(&components_lock);
    return err;
}

err_t set_primary(component_t* comp) {
    spinlock_acquire(&components_lock);
    primary_components[comp->type] = comp;
    spinlock_release(&components_lock);
    return NO_ERROR;
}