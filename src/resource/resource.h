#ifndef TOMATKERNEL_RESOURCE_H
#define TOMATKERNEL_RESOURCE_H

#include <error.h>

#define RESOURCE_FILE   0
#define RESOURCE_DIR    1

#define RESOURCE_READ   (1 << 0)
#define RESOURCE_WRITE  (1 << 1)
#define RESOURCE_SEEK   (1 << 2)

#define SEEK_START  0
#define SEEK_END    1
#define SEEK_CUR    2

typedef struct resource_descriptor {
    const char* scheme;
    const char* domain;
    int port;
    const char* path;
    struct resource_descriptor* sub;
} resource_descriptor_t;

typedef struct resource_stat {
    char name[255];
    uint8_t type;
    uint8_t read_only : 1;
    uint8_t link : 1;
    uint64_t time_created;
    uint64_t time_modified;
    uint64_t time_accessed;
} resource_stat_t;

/*
 * This is the kernel's resource handler
 *
 * it is a global handle meaning it can be shared between any part of the kernel, unlike
 * a user space resource handle which has a unique id per process
 */
typedef uint64_t kresource_t;

/**
 * Open a resource
 *
 * @param descriptor    [IN]    The descriptor to open (must have the scheme specified)
 * @param out_res       [OUT]   The new resource
 */
error_t kopen(resource_descriptor_t* descriptor, kresource_t* out_res);

/**
 * Close the resource, freeing it from the process
 *
 * @param res   [IN] The resource to free
 */
error_t kclose(kresource_t res);

/**
 * Seek into a resource
 *
 * @param res       [IN] The resource to seek in
 * @param offset    [IN] The offset to seek
 * @param dir       [IN] The direction (from start, end and so on)
 *
 * @remark
 * The dir accepts:                                                                                                 <br>
 * - SEEK_START: new offset = offset                                                                                <br>
 * - SEEK_END: new offset = file size - offset                                                                      <br>
 * - SEEK_CUR: new offset = old offset + offset
 */
error_t kseek(kresource_t res, size_t offset, int dir);

/**
 * Get the pointer of the resource
 *
 * @param res       [IN]    The resource to get the pointer of
 * @param offset    [OUT]   The pointer's offset from the start of the resource
 */
error_t ktell(kresource_t res, size_t* offset);

/**
 * Read from a resource
 *
 * @param res           [IN]    The resource to read from
 * @param buffer        [OUT]   The buffer to write to
 * @param buffer_size   [IN]    The size of the given buffer
 * @param read_size     [OUT]   The amount actually read
 */
error_t kread(kresource_t res, void* buffer, size_t buffer_size, size_t read_size);

/**
 * Write to a resource
 *
 * @param res           [IN]    The resource to write to
 * @param buffer        [IN]    The buffer we want to write from
 * @param buffer_size   [IN]    The size of the buffer we wanna write
 * @param writen_size   [OUT]   The amount actually written
 */
error_t kwrite(kresource_t res, const void* buffer, size_t buffer_size, size_t writen_size);

/**
 * State a resource
 *
 * @param res   [IN]    The resource to stat
 * @param stat  [OUT]   The stat of the resource
 * @return
 */
error_t kstat(kresource_t res, resource_stat_t* stat);

/**
 * Poll on the state of a resource
 *
 * @param res   [IN]    The resource to poll on
 * @param state [OUT]   The state and what we can do with the resource
 *
 * @remark
 * The state is a bitmap of:                                                                                        <br>
 * - RESOURCE_READ: You can read                                                                                    <br>
 * - RESOURCE_WRITE: You can write                                                                                  <br>
 * - RESOURCE_SEEK: You can seek
 */
error_t kpoll(kresource_t res, int* state);

/**
 * Make a new directory for the resource descriptor
 *
 * @param descriptor [IN] The descriptor
 */
error_t kmkdir(resource_descriptor_t* descriptor);

/**
 * Read the next entry in the directory
 *
 * @param res       [IN]        The resource to read the dir from
 * @param stat      [IN/OUT]    The stat of the resource
 * @param index     [IN]        The index of the entry to read (This does not need to be sorted)
 */
error_t kreaddir(kresource_t res, resource_stat_t* stat, int index);

#endif //TOMATKERNEL_RESOURCE_H
