#ifndef TOMATOS_DRIVER_H
#define TOMATOS_DRIVER_H

#include <util/except.h>
#include <stdint.h>

#include <lai/core.h>
#include <lai/internal-ns.h>

typedef enum driver_bind_type {
    DRIVER_END,

    DRIVER_ACPI,
} driver_bind_type_t;

typedef struct driver_bind {
    driver_bind_type_t type;
    union {
        struct {
            const char* hid;
        } acpi;
    };
} driver_bind_t;

typedef struct driver_bind_data {
    driver_bind_t* bind;
    union {
        struct {
            lai_nsnode_t* node;
        } acpi;
    };
} driver_bind_data_t;

typedef err_t (*driver_func_t)(driver_bind_data_t* data);

typedef struct driver {
    const char* name;
    driver_func_t entry;
    driver_bind_t* binds;
} driver_t;

/**
 * Driver dispatch
 */
err_t dispatch_drivers();

/**
 * The kernels compiled in the kernel
 */
extern driver_t g_drivers[];
extern driver_t g_drivers_end[];

/**
 * Kinda ugly but works lol
 */
#define DRIVER __attribute__((used, section(".drivers"))) static driver_t CONCAT(__driver_, __LINE__) =

#endif //TOMATOS_DRIVER_H
