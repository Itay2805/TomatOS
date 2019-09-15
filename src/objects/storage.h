#ifndef TOMATKERNEL_STORAGE_H
#define TOMATKERNEL_STORAGE_H

#include <common/error.h>
#include "object.h"

typedef enum storage_syscalls {
    STORAGE_SYS_READ,
    STORAGE_SYS_WRITE,
    STORAGE_SYS_GET_SECTOR_SIZE,

    STORAGE_SYS_MAX
} storage_syscalls_t;

typedef struct storage_functions {
    error_t (*read)(object_t* obj, void* buffer, size_t length, size_t offset);
    error_t (*write)(object_t* obj, void* buffer, size_t length, size_t offset);
    error_t (*get_sector_size)(object_t* obj, size_t* buffer);
} storage_functions_t;

#endif //TOMATKERNEL_STORAGE_H
