#ifndef TOMATOS_HANDLE_H
#define TOMATOS_HANDLE_H

#include <util/defs.h>
#include <stdatomic.h>

/**
 * Represents an invalid handle
 */
#define NULL_HANDLE (0)

/**
 * Represents a handle to the current process
 */
#define THIS_PROCESS (-1)

/**
 * Represents a handle to the current thread
 */
#define THIS_THREAD (-2)

/**
 * Handle used in kernel (and userspace)
 */
typedef int64_t handle_t;

typedef enum handle_kind {
    HANDLE_PROCESS,
    HANDLE_THREAD,
} handle_kind_t;

struct handle_meta;
typedef err_t (*handle_dtor_t)(struct handle_meta* meta);

/**
 * The metadata of a handle
 */
typedef struct handle_meta {
    /**
     * The kind of handle
     */
    handle_kind_t kind;

    /**
     * The refcount of the handle
     */
    atomic_int refcount;

    /**
     * The destructor of the handle
     */
    handle_dtor_t dtor;
} handle_meta_t;

/**
 * This will release the meta
 *
 * @param meta          [IN]    The meta to release
 */
err_t release_handle_meta(handle_meta_t* meta);

#endif //TOMATOS_HANDLE_H
