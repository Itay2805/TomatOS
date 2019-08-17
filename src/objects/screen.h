#ifndef TOMATKERNEL_SCREEN_H
#define TOMATKERNEL_SCREEN_H

#include <common/error.h>
#include "object.h"

typedef enum screen_function_type {
    SCREEN_FUNCTION_CLEAR,
    SCREEN_FUNCTION_COUNT,
} screen_function_type_t;

typedef struct screen_functions {
    error_t (*clear)(object_t* obj);
} screen_functions_t;

#endif //TOMATKERNEL_SCREEN_H
