#include <stb/stb_ds.h>
#include "driver.h"

static khandle_t handle_gen = 1;

driver_entry_t* drivers;
khandle_entry_t* khandles;

error_t driver_register(const char* scheme, driver_t* driver) {
    error_t err = NO_ERROR;

    CHECK_TRACE(shgeti(drivers, scheme) == 0, "The scheme `%s` is already registered", scheme);
    shput(drivers, scheme, driver);

cleanup:
    return err;
}

khandle_t driver_create_handle(driver_t* driver) {
    khandle_t handle = handle_gen++;
    hmput(khandles, handle, driver);
    return handle;
}

error_t kopen(resource_descriptor_t* descriptor, khandle_t* handle) {
    driver_t* driver = shget(drivers, descriptor->scheme);
    return driver->class_functions.open(descriptor, handle);
}

error_t kclose(khandle_t handle) {
    driver_t* driver = hmget(khandles, handle);
    return driver->handle_functions.close(handle);
}
