#ifndef __COMPO_COMPONENT_H__
#define __COMPO_COMPONENT_H__

#include <util/except.h>
#include <util/list.h>

#include <stdatomic.h>
#include <stdint.h>
#include <stddef.h>

typedef struct address {
    uint32_t data1;
    uint16_t data2;
    uint16_t data3;
    uint8_t data4[8];
} address_t;

typedef enum component_type {
    COMPONENT_FILESYSTEM,

    COMPONENT_MAX_TYPE
} component_type_t;

typedef struct component {
    // the component type
    component_type_t type;

    // the unique component address (saved across reboots)
    address_t address;

    // the link in the component list
    list_entry_t link;

    // the link of the specific component
    list_entry_t type_link;

    // the reference count
    atomic_int ref_count;

    // delete the component
    err_t (*delete)(struct component* comp);
} component_t;

/**
 * Will generate an address from the given buffer
 *
 * This will simply run sha1 on the buffer
 *
 * @param address   [IN]    The buffer to calculate the address from
 * @param len       [IN]    The length of the buffer
 * @param addr      [OUT]   The out address
 */
void generate_device_address(void* address, size_t len, address_t* addr);

/**
 * Register a new component
 *
 * @param component [IN] The component to register
 */
void register_component(component_t* component);

/**
 * Get a component by it's address
 *
 * @remark
 * Use release after you are done with it
 *
 * @param address   [IN]    The address to find
 * @param comp      [OUT]   The component with that address
 * @return
 */
err_t get_component(address_t address, component_t** comp);

/**
 * will release the component
 *
 * @param comp  [IN] The component to release
 */
err_t release_component(void* comp);

/**
 * Get the next component of a certain type
 *
 * If the component is NULL will start with the first component
 *
 * @remark
 * Use release after you are done with it
 *
 * @param type  [IN]    The type of component to search for
 * @param comp  [OUT]   The component
 */
err_t get_next_component(component_type_t type, component_t** comp);

/**
 * Get the primary component of a certain type
 *
 * @remark
 * Use release after you are done with it
 *
 * @param type  [IN] The type of component
 * @param comp  [IN] The primary component
 */
err_t get_primary(component_type_t type, component_t** comp);

/**
 * Set the primary component of a certain type
 *
 * @param comp  [IN] The component to set to primary
 */
err_t set_primary(component_t* comp);

#endif //__COMPO_COMPONENT_H__
