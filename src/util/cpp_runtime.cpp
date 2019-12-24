#include "cpp_runtime.h"
#include "debug.h"


#ifdef __cplusplus
extern "C" {
#endif

int __cxa_atexit(void (*f)(void *), void *objptr, void *dso) {
    return 0;
}

#ifdef __cplusplus
}
#endif

namespace util {

    typedef void(*ctor_func)(void);

    extern "C" void* kernel_ctors_end;
    extern "C" void* kernel_ctors_start;
    #define KERNEL_CTORS_START ((uint64_t)&kernel_ctors_start)
    #define KERNEL_CTORS_END ((uint64_t)&kernel_ctors_end)
    #define KERNEL_CTORS_SIZE (KERNEL_CTORS_END - KERNEL_CTORS_START)

    void call_ctors() {
        ctor_func* kernel_ctors = (ctor_func*)KERNEL_CTORS_START;
        for(int i = 0; i < KERNEL_CTORS_SIZE / sizeof(void*); i++) {
            kernel_ctors[i]();
        }
    }

}
