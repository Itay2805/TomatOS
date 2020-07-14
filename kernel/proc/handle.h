#ifndef __TOMATOS_KERNEL_PROC_HANDLE_H__
#define __TOMATOS_KERNEL_PROC_HANDLE_H__

#include <util/except.h>
#include <stdatomic.h>
#include <stdint.h>

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
    HANDLE_EVENT,
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

// TODO: maybe allow for doing handle stuff on other processes

/**
 * Create a handle in the current process
 *
 * @remark
 * This will increment the refcount of the meta
 *
 * @param meta      [IN]    The meta to bind for the handle
 * @param handle    [OUT]   The handle of the process
 */
err_t create_handle(handle_meta_t* meta, handle_t* handle);

/**
 * Will copy a handle to another process
 *
 * @remark
 * The out handle is not valid in the current process, but in the current
 * process, of course if THIS_PROCESS is used then it will be valid
 *
 * @param target    [IN]    The target process
 * @param in        [IN]    The input handle to copy
 * @param out       [OUT]   The handle (of the other process)
 */
err_t copy_handle(handle_t target, handle_t in, handle_t* out);

/**
 * Get the metadata of a handle
 *
 * @remark
 * This will increase the refcount of the meta, you have to release
 * it using the release_handle_meta.
 *
 * @remark
 * This can only get the meta for the current running process
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
 * @remark
 * This will close the handle in the context of the running
 * process
 *
 * @param handle        [IN]    The handle to close
 */
err_t close_handle(handle_t handle);


#endif //__TOMATOS_KERNEL_PROC_HANDLE_H__
