#ifndef TOMATOS_DRIVER_H
#define TOMATOS_DRIVER_H

#include <proc/handle.h>
#include <util/except.h>
#include <util/list.h>
#include <lai/core.h>

#include <stdint.h>

#include "pci/pci.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Driver interfaces
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct driver_instance;

typedef enum driver_interface_type {
    DRIVER_KEYBOARD,
    DRIVER_MOUSE,
    DRIVER_STORAGE,
    DRIVER_BLOCK,
    DRIVER_FS,
    DRIVER_MAX,
} driver_interface_type_t;

typedef struct driver_block {
    // information about the block
    size_t block_size;
    size_t last_block;

    // functions to modify the block device
    err_t (*read_blocks)(struct driver_instance* instance, size_t lba, void* buffer, size_t buffer_size);
    err_t (*write_blocks)(struct driver_instance* instance, size_t lba, void* buffer, size_t buffer_size);
    err_t (*flush_blocks)(struct driver_instance* instance);
} driver_block_t;

typedef err_t (*driver_connect_t)(struct driver_instance* instance);
typedef bool (*driver_check_t)(struct driver_instance* instance);

typedef struct driver_interface {
    driver_interface_type_t type;
} driver_interface_t;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Driver bindings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef enum driver_bind_type {
    BIND_END,
    BIND_ACPI,
    BIND_PCI,
    BIND_INTERFACE,
    BIND_FALLBACK,
} driver_bind_type_t;

typedef struct driver_bind {
    driver_bind_type_t type;
    int priority;
    union {
        struct {
            const char* hid;
        } acpi;
        struct {
            uint16_t device_id;
            uint16_t vendor_id;
            uint8_t class;
            uint8_t subclass;
            uint8_t progif;
        } pci;
        struct {
            driver_interface_type_t type;
            // TODO: why are we actually using connect and not
            //       the default entry?
            driver_connect_t connect;
            driver_check_t check;
        } interface;
        struct {
            driver_interface_type_t type;
        } fallback;
    };
} driver_bind_t;

typedef struct driver_bind_data {
    driver_bind_t* bind;
    union {
        lai_nsnode_t* acpi_node;
        pci_dev_t* pci_dev;
    };
} driver_bind_data_t;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Driver definition
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef err_t (*driver_func_t)(driver_bind_data_t* data);

typedef struct driver {
    const char* name;
    driver_func_t entry;
    driver_interface_t interface;
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
 * Declare a driver with lowest priority
 */
#define DRIVER \
    __attribute__((used, section(".drivers"))) static driver_t CONCAT(__driver_, __LINE__) =

/**
 * Declare a driver with high priority
 */
#define PRIORITY_DRIVER \
    __attribute__((used, section(".drivers.priority"))) static driver_t CONCAT(__driver_, __LINE__) =

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Driver definition
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct driver_instance {
    /**
     * The driver handle
     */
    handle_meta_t meta;

    /**
     * The name of the instance
     */
    char name[50];

    /**
     * The link to the driver list
     */
    list_entry_t link;

    /**
     * The type of interface
     */
    driver_interface_type_t type;

    /**
     * The interface itself
     */
    union {
        driver_block_t block;
    };
} driver_instance_t;

/**
 * The existing interfaces
 */
extern list_entry_t g_interfaces[DRIVER_MAX];

/**
 * Locks for each of the interfaces lists
 * TODO: replace with rwlock
 */
extern ticket_lock_t g_interfaces_locks[DRIVER_MAX];

/**
 * Initialize all the interfaces
 */
void init_driver_interface();

/**
 * Register a driver interface
 */
err_t register_interface(driver_instance_t* instance);

#endif //TOMATOS_DRIVER_H