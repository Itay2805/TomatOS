#ifndef TOMATKERNEL_KLIB_H
#define TOMATKERNEL_KLIB_H

#include "stdbool.h"
#include <resource/resource.h>

#define SEEK_CUR 0
#define SEEK_START 1
#define SEEK_END 2

bool open(resource_descriptor_t* desc, resource_t* resource);
bool read(resource_t res, void* buffer, int size, int* outSize);
bool write(resource_t res, const void* buffer, int size, int* outSize);
bool seek(resource_t res, int relative, ptrdiff_t offset);
bool tell(resource_t res, uint64_t* offset);
bool invoke(resource_t res, int command, void* arg);
bool close(resource_t res);

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
