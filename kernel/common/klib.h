#ifndef TOMATKERNEL_KLIB_H
#define TOMATKERNEL_KLIB_H

#include "stdbool.h"
#include <resource/resource.h>

#define SEEK_CUR 0
#define SEEK_START 1
#define SEEK_END 2

error_t kopen(resource_descriptor_t* desc, resource_t* resource);
error_t kread(resource_t res, void* buffer, int size, size_t* outSize);
error_t kwrite(resource_t res, const void* buffer, int size, size_t* outSize);
error_t kseek(resource_t res, int relative, ptrdiff_t offset);
error_t ktell(resource_t res, uint64_t* offset);
error_t kinvoke(resource_t res, uint64_t command, void* arg);
error_t kclose(resource_t res);
error_t kpoll(resource_t res);
bool wait(resource_t res);

void fprintf(resource_t res, const char* fmt, ...)  __attribute__ ((format (printf, 2, 3)));

/**
 * Will kill the thread with the given tid, use 0 to kill the current thread
 *
 * @param tid   [IN] The thread to kill
 *
 * @return
 * false on failure, true on success
 * (will not return if killing the current thread)
 */
bool tkill(int tid);

#endif //TOMATKERNEL_KLIB_H
