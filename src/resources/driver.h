#ifndef TOMATOS_DRIVER_H
#define TOMATOS_DRIVER_H

#include <error.h>
#include "resource.h"

typedef error_t (*driver_open_f)(resource_descriptor_t* descriptor, khandle_t* handle);
typedef error_t (*driver_close_f)(khandle_t handle);

typedef struct driver {

    struct {
        driver_open_f open;
    } class_functions;

    struct {
        driver_close_f close;
    } handle_functions;
} driver_t;

typedef struct driver_entry {
    const char* key;
    driver_t* value;
} driver_entry_t;

typedef struct khandle_entry {
    khandle_t key;
    driver_t* value;
} khandle_entry_t;

// map<string, driver>
extern driver_entry_t* drivers;

// map<khandle, driver>
extern khandle_entry_t* khandles;

/**
 * Register a new driver
 *
 * @remark
 * The driver must be kept alive by the driver
 *
 * @param scheme    [IN] The scheme name of the driver
 * @param driver    [IN] The driver itself
 */
error_t driver_register(const char* scheme, driver_t* driver);

/**
 * Create a new handle for the given driver
 */
khandle_t driver_create_handle(driver_t* driver);

error_t kopen(resource_descriptor_t* descriptor, khandle_t* handle);
error_t kclose(khandle_t handle);

#endif //TOMATOS_DRIVER_H
