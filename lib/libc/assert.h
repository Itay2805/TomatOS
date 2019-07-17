#ifndef TOMATKERNEL_ASSERT_H
#define TOMATKERNEL_ASSERT_H

#include "stdbool.h"

// TODO: just have a panic function
#define assert(x) \
    do { \
        if(!(x)) { \
            log_critical("Kernel panic failed! (%s:%d)", __FILENAME__, __LINE__); \
            _cli(); \
            while(true) { \
                _hlt(); \
            } \
        } \
    } while(0);

#endif //TOMATKERNEL_ASSERT_H