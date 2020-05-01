#include <crypto/sha1.h>
#include <sync/spinlock.h>
#include <string.h>
#include "component.h"

// list with all components
static spinlock_t g_components_lock = SPINLOCK_INIT;
static list_entry_t g_all_components = INIT_LIST_ENTRY(g_all_components);

// the primary of each component type
static component_t* g_primary_components[COMPONENT_MAX_TYPE] = {0};
static list_entry_t g_per_type_components[COMPONENT_MAX_TYPE];

// component type to name map
static const char* type_to_name[] = {
    [COMPONENT_DISK] = "Disk",
    [COMPONENT_BLOCK] = "Block",
    [COMPONENT_FILESYSTEM] = "Filesyste,",
    [COMPONENT_DISPLAY] = "Display",
    [COMPONENT_MOUSE] = "Mouse",
    [COMPONENT_KEYBOARD] = "Keyboard",
};

static void components_lock() {
    spinlock_acquire(&g_components_lock);
}

static void components_unlock() {
    spinlock_release(&g_components_lock);
}

static err_t decrement_component_refcount(component_t* c) {
    err_t err = NO_ERROR;

    CHECK_ERROR(c != NULL, ERROR_INVALID_PARAM);

    // decrement the ref count and if zero free it
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

void init_components() {
    for (int i = 0; i < COMPONENT_MAX_TYPE; i++) {
        g_per_type_components[i] = INIT_LIST_ENTRY(g_per_type_components[i]);
    }
}

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
    list_entry_t* list = &g_per_type_components[component->type];

    // insert it
    components_lock();
    insert_tail_list(list, &component->type_link);
    insert_tail_list(&g_all_components, &component->link);
    components_unlock();

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

    components_lock();

    FOR_EACH_IN_LIST(g_all_components, link) {
        component_t* component = CR(link, component_t, link);
        component->ref_count++;
        *comp = component;
        goto cleanup;
    }

    CHECK_FAIL_ERROR(ERROR_NOT_FOUND);

cleanup:
    components_unlock();
    return err;
}

err_t release_component(void* comp) {
    err_t err = NO_ERROR;
    component_t* c = comp;

    components_lock();

    CHECK_ERROR(c != NULL, ERROR_INVALID_PARAM);

    decrement_component_refcount(c);

cleanup:
    components_unlock();
    return err;
}

err_t get_next_component(component_type_t type, component_t** comp) {
    err_t err = NO_ERROR;

    components_lock();

    CHECK_ERROR(comp != NULL, ERROR_INVALID_PARAM);

cleanup:
    components_unlock();
    return err;
}

err_t get_primary(component_type_t type, component_t** comp) {
    err_t err = NO_ERROR;

    components_lock();

    CHECK_ERROR(comp != NULL, ERROR_INVALID_PARAM);
    CHECK_ERROR(g_primary_components[type] != NULL, ERROR_NOT_FOUND);

    // increment the ref count and set it
    g_primary_components[type]->ref_count++;
    *comp = g_primary_components[type];

cleanup:
    components_unlock();
    return err;
}

err_t set_primary(component_t* comp) {
    err_t err = NO_ERROR;

    components_lock();

    // if already has one release it
    if (g_primary_components[comp->type] != NULL) {
        decrement_component_refcount(g_primary_components[comp->type]);
    }

    // increment the count of this one
    comp->ref_count++;
    g_primary_components[comp->type] = comp;

cleanup:
    components_unlock();
    return err;
}