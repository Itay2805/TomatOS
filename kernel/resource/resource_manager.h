#ifndef TOMATKERNEL_RESOURCE_MANAGER_H
#define TOMATKERNEL_RESOURCE_MANAGER_H

#include <common/except.h>

struct process;

typedef int resource_t;

typedef struct resource_provider {
    int pid;
    const char* scheme;

    void* open;
    void* read;
    void* write;
    void* seek;
    void* tell;
    void* poll;
    void* close;
} resource_provider_t;

/**
 * Initializes the resource manager
 */
error_t resource_manager_init();

/**
 * Will register a provider to the providers list
 *
 * @param provider [IN] Pointer to the provider
 *
 * @remark
 * It is up to the provider to keep the pointer valid, best way is to simply have it statically allocated in the kernel
 */
error_t resource_manager_register_provider(resource_provider_t* provider);

/**
 * Get a resource provider by the resource and process the resource is attached to
 *
 * @param process   [IN]    The process the resource belongs to
 * @param resource  [IN]    The resource to get the provider for
 * @param provider  [OUT]   The pointer to the provider struct
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