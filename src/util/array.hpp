#pragma once

#include <stddef.h>
#include "debug.h"

namespace util {

    template<typename T, int count>
    class array {
    private:
        T entries[count];

    public:

        T& operator[](int index) {
            ASSERT(0 <= index && index < count);
            return this->entries[index];
        }

        const T& operator[](int index) const {
            ASSERT(0 <= index && index < count);
            return this->entries[index];
        }

        inline size_t length() const { return count; }

    };

}
