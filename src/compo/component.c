#include <crypto/sha1.h>
#include <sync/spinlock.h>
#include <string.h>
#include "component.h"

// list with specific components
static list_entry_t fs_components = INIT_LIST_ENTRY(fs_components);

// list with all components
static spinlock_t components_lock = SPINLOCK_INIT;
static list_entry_t all_components = INIT_LIST_ENTRY(all_components);

// the primary of each component type
static component_t* primary_components[COMPONENT_MAX_TYPE] = {0};

// component type to name map
static const char* type_to_name[] = {
    [COMPONENT_FILESYSTEM] = "Filesystem"
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
        if (memcmp(&address, &component->address, 16) == 0) {
            *comp = component;
            goto cleanup;
        }
    }

    CHECK_FAIL_ERROR(ERROR_NOT_FOUND);

cleanup:
    spinlock_release(&components_lock);
    return err;
}

err_t get_next_component(component_type_t type, component_t** comp) {
    return ERROR_NOT_READY;
}

err_t get_primary(component_type_t type, component_t** comp) {
    err_t err = NO_ERROR;

    spinlock_acquire(&components_lock);

    CHECK_ERROR(comp != NULL, ERROR_INVALID_PARAM);
    CHECK_ERROR(primary_components[type] != NULL, ERROR_NOT_FOUND);
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