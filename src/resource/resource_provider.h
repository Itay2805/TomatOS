#ifndef TOMATKERNEL_RESOURCE_PROVIDER_H
#define TOMATKERNEL_RESOURCE_PROVIDER_H

#include "resource.h"

typedef error_t (*resource_open)(resource_descriptor_t* descriptor, kresource_t* out_res);
typedef error_t (*resource_close)(kresource_t res);
typedef error_t (*resource_seek)(kresource_t res, size_t offset, int dir);
typedef error_t (*resource_tell)(kresource_t res, size_t* offset);
typedef error_t (*resource_read)(kresource_t res, void* buffer, size_t buffer_size, size_t read_size);
typedef error_t (*resource_write)(kresource_t res, const void* buffer, size_t buffer_size, size_t writen_size);
typedef error_t (*resource_stat)(kresource_t res, resource_stat_t* stat);
typedef error_t (*resource_poll)(kresource_t res, int* state);
typedef error_t (*resource_mkdir)(resource_descriptor_t* descriptor);
typedef error_t (*resource_readdir)(kresource_t res, resource_stat_t* stat, int index);

typedef struct resource_provider {
    const char* scheme;

    // TODO: Attechment to a process to allow for the creation of threads

    resource_open open;
    resource_close close;
    resource_seek seek;
    resource_tell tell;
    resource_read read;
    resource_write write;
    resource_stat stat;
    resource_poll poll;
    resource_mkdir mkdir;
    resource_readdir readdir;
} resource_provider_t;

/**
 * Register a new provider which can be used
 *
 * @remark
 * This will only save the pointer, so the struct itself must be kept valid by
 * the called.
 *
 * @param provider  [IN] The provider to save
 */
error_t register_provider(resource_provider_t* provider);

/**
 * Get the provider by the scheme name
 *
 * @param scheme    [IN] The name of the scheme
 */
resource_provider_t* get_provider_by_scheme(const char* scheme);

/**
 * Get the provider by a kernel resource handle
 *
 * @param res       [IN] The kernel resource handle
 */
resource_provider_t* get_provider_by_kresource(kresource_t res);

#endif //TOMATKERNEL_RESOURCE_PROVIDER_H
