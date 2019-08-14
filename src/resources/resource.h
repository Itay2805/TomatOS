#ifndef TOMATKERNEL_ESOURCE_H
#define TOMATKERNEL_ESOURCE_H

#include <stdint.h>
#include <tomato/resource.h>

/**
 * This is a kernel handle, the kernel uses this internally to track all resources
 *
 * in user space different file descriptors may reference the same handle
 */
typedef uint64_t khandle_t;

#endif //TOMATKERNEL_ESOURCE_H
