#ifndef __COMPO_COMPONENT_H__
#define __COMPO_COMPONENT_H__

#include <stdint.h>
#include <stddef.h>

#include <util/list.h>
#include <util/except.h>

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
} component_t;


void generate_device_address(void* address, size_t len, address_t* addr);

void register_component(component_t* component);

err_t get_component(address_t address, component_t** comp);

err_t get_next_component(component_type_t type, component_t** comp);

err_t get_primary(component_type_t type, component_t** comp);

err_t set_primary(component_t* comp);

#endif //__COMPO_COMPONENT_H__
