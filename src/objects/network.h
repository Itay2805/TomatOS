#ifndef TOMATKERNEL_NETWORK_H
#define TOMATKERNEL_NETWORK_H

#include "object.h"

typedef struct network_functions {
    void (*send)(object_t* obj, void* buffer, size_t size);
} network_functions_t;

#endif //TOMATKERNEL_NETWORK_H
