#pragma once

#include <stddef.h>


namespace util {

    /**
     * Will call all the constructors
     *
     * they assume that the vmm is initialized and that they can allocate
     * memory safely
     */
    void call_ctors();

}