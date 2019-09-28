#ifndef TOMATKERNEL_NETWORK_H
#define TOMATKERNEL_NETWORK_H

#include "object.h"

typedef struct network_functions {
    error_t (*get_mac)(object_t* obj, mac_t* mac);
    error_t (*send)(object_t* obj, void* buffer, size_t size);
} network_functions_t;

#endif //TOMATKERNEL_NETWORK_H
