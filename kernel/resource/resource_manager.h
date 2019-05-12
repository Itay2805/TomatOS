#ifndef TOMATKERNEL_RESOURCE_MANAGER_H
#define TOMATKERNEL_RESOURCE_MANAGER_H

#include <common/except.h>
#include <process/thread.h>

struct process;
struct resource_descriptor;

typedef int resource_t;

typedef struct resource_provider {
    int pid;
    const char* scheme;

    bool wait_support;

    error_t (*open)(struct process* process, thread_t* thread, struct resource_descriptor* descriptor, resource_t* resource);
    error_t (*read)(struct process* process, thread_t* thread, resource_t resource, char* buffer, size_t len, size_t* read_len);
    error_t (*write)(struct process* process, thread_t* thread, resource_t resource, const char* buffer, size_t len, size_t* write_len);
    error_t (*seek)(struct process* process, thread_t* thread, resource_t resource, int relative, ptrdiff_t pos);
    error_t (*tell)(struct process* process, thread_t* thread, resource_t resource, size_t* pos);
    error_t (*poll)(struct process* process, thread_t* thread, resource_t resource);
    error_t (*close)(struct process* process, thread_t* thread, resource_t resource);
    error_t (*invoke)(struct process* process, thread_t* thread, resource_t res, uint64_t cmd, void* arg);
} resource_provider_t;

/**
 * Initializes the resource manager
 */
error_t resource_manager_init();

/**
 * Will tell the resource manager that the resource is ready
 * 
 * @param process   [IN] The process the resource belongs to
 * @param resource  [IN] The resource which is ready
 */
error_t resource_manager_resource_ready(thread_t* thread, resource_t resource);

/**
 * Will register a provider to the providers list
 *
 * @param provider [IN] Pointer to the provider
 *
 * @remark
 * It is up to the provider to keep the pointer valid, best way is to simply have it statically allocated in
 * the kernel
 *
 * @remark
 * This function should only be called during boot
 */
error_t resource_manager_register_provider(resource_provider_t* provider);

/**
 * Get a resource provider by the resource and process the resource is attached to
 *
 * @param process   [IN]    The process the resource belongs to
 * @param resource  [IN]    The resource to get the provider for
 * @param provider  [OUT]   The pointer to the provider struct
 *
 * @remark
 * You should never call this from context with interrupts disabled (interrupt handler or critical section)
 */
error_t resource_manager_get_provider_by_resource(struct process* process, resource_t resource, resource_provider_t** provider);

/**
 * Get a resource provider by it's scheme name
 *
 * @param scheme    [IN]    The name of the scheme
 * @param provider  [OUT]   The pointer to the provider struct
 */
error_t resource_manager_get_provider_by_scheme(const char* scheme, resource_provider_t** provider);

/**
 * Get a resource provider by the pid of the handling process
 *
 * @param pid       [IN]    The pid of the process
 * @param provider  [OUT]   The pointer to the provider struct
 *
 * @remark
 * TODO: Right now it means that a provider is per process which is not the best
 */
error_t resource_manager_get_provider_by_pid(int pid, resource_provider_t** provider);

#endif