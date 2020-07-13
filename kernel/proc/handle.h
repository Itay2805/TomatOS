#ifndef __TOMATOS_KERNEL_PROC_HANDLE_H__
#define __TOMATOS_KERNEL_PROC_HANDLE_H__

#include <util/except.h>
#include <stdint.h>
#include <stdatomic.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// A handle has a specific structure
//  * first 62bits - the handle id
//  * last 2bits - handle type:
//          * 0 - handle local to current process
//          * 1 - handle global to the system
//          * 2 - special handle that changes per-context
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Represents an invalid handle
 */
#define NULL_HANDLE (0)

/**
 * Represents a handle to the current process
 */
#define THIS_PROCESS ((2) << 62 | 1)

/**
 * Represents a handle to the current thread
 */
#define THIS_THREAD ((2) << 62 | 2)

/**
 * Handle used in kernel (and userspace)
 */
typedef uint64_t handle_t;


typedef enum handle_kind {
    /**
     * A process handle
     */
    HANDLE_PROCESS,

    /**
     * A thread handle
     */
    HANDLE_THREAD,
} handle_kind_t;


/**
 * The metadata of a handle
 */
typedef struct handle_meta {
    /**
     * The size of the handle (including meta)
     */
    size_t size;

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
    err_t (*dtor)(struct handle_meta* meta);

    /**
     * The metadata of the handle
     */
    char meta[];
} handle_meta_t;

/**
 * This will create a global handle and bind it to a meta object
 *
 * @remark
 * Teh dtor needs to know how to actually free the handle
 *
 * @param meta          [IN]    The meta object to bind
 * @param out_handle    [OUT]   The output handle
 */
err_t create_and_bind_global_handle(handle_meta_t* meta, handle_t* out_handle);

/**
 * Get the metadata of a handle
 *
 * @remark
 * This will increase the refcount of the meta, you have to release
 * it using the release_handle_meta.
 *
 * @param handle        [IN]    The handle to get the meta of
 * @param meta          [OUT]   The meta of the handle
 */
err_t get_handle_meta(handle_t handle, handle_meta_t** meta);

/**
 * This will release the meta
 *
 * @param meta          [IN]    The meta to release
 */
err_t release_handle_meta(handle_meta_t* meta);

/**
 * Close a handle
 *
 * @remark
 * In reality this decreases the refcount of the handle
 *
 * @param handle        [IN]    The handle to close
 */
err_t close_handle(handle_t handle);


#endif //__TOMATOS_KERNEL_PROC_HANDLE_H__
