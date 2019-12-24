#pragma once

#include <stddef.h>

// Placement new operators
// copied from https://wiki.osdev.org/C++
inline void *operator new(size_t, void *p) { return p; }
inline void *operator new[](size_t, void *p) { return p; }
inline void  operator delete  (void *, void *) { };
inline void  operator delete[](void *, void *) { };

namespace util {

    /**
     * Will call all the constructors
     *
     * they assume that the vmm is initialized and that they can allocate
     * memory safely
     */
    void call_ctors();

}