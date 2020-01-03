#include <mem/mm.hpp>
#include "cpp_runtime.h"
#include "debug.hpp"

extern "C" int __cxa_atexit(void (*f)(void *), void *objptr, void *dso) {
    return 0;
}

extern "C" void __cxa_pure_virtual() {
    ASSERT(false);
}

void *operator new(size_t size) {
    return mem::allocator.allocate(size);
}

void *operator new[](size_t size) {
    return mem::allocator.allocate(size);
}

void operator delete(void *p) {
    mem::allocator.free(p);
}

void operator delete[](void *p) {
    mem::allocator.free(p);
}

extern void(*__init_array_start [])();
extern void(*__init_array_end [])();

namespace util {

    void call_ctors() {
        int count = __init_array_end - __init_array_start;
        for (int i = 0; i < count; i++) {
            __init_array_start[i]();
        }
    }

}
