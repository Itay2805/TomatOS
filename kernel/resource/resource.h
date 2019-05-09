#ifndef TOMATKERNEL_RESOURCE_H
#define TOMATKERNEL_RESOURCE_H

#include <resource/resource_manager.h>

#include <common/except.h>

// TODO: Use this, basically the first few bits are gonna be used just for
//       storing the resource information (will not require a syscall to query
//       these stuff)
#define RESOURCE_ATTR_SEEKABLE (1 << 0)
#define RESOURCE_ATTR_READABLE (1 << 1)
#define RESOURCE_ATTR_WRITEABLE (1 << 2)
#define RESOURCE_ATTR_WAITABLE (1 << 3)

typedef int resource_t;

typedef struct resource_descriptor {
    char* scheme;
    char* domain;
    char* path;
    int port;
    struct resource_descriptor* sub;
} resource_descriptor_t;

// forward declare
struct process;

/**
 * Will create a new resource for the provider
 *
 * @param process   [IN]    The process to add the resource to
 * @param provider  [IN]    The provider that needs the resource
 * @param resource  [OUT]   The resource created
 */
error_t resource_create(struct process* process, resource_provider_t* provider, resource_t* resource);

/**
 * Will hash the pid and resource to give out a unique hash
 *
 * this is for using in maps
 */
uint64_t hash_resource(int pid, resource_t resource);

/**
 * Will fully copy the descriptor to the kernel
 *
 * @param original_process  [IN]    The original process the descriptor is from
 * @param descriptor        [IN]    The descriptor
 * @param resource          [OUT]   The copied one
 *
 * @remark
 * Don't forget to delete the resource descriptor with `delete_resource_descriptor`
 */
error_t copy_resource_descriptor_to_kernel(struct process* original_process, resource_descriptor_t* descriptor, resource_descriptor_t** resource);

/**
 * Will delete the descriptor from the kernel
 *
 * @param resource  [IN]    The descriptor to delete
 */
error_t delete_resource_descriptor(resource_descriptor_t* resource);

#endif
