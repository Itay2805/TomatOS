#pragma once

#include <stdint.h>

namespace arch::sync {

    uint32_t interlocked_increment(_Atomic uint32_t* value);
    uint32_t interlocked_decrement(_Atomic uint32_t* value);
    uint32_t interlocked_compare_exchange(_Atomic uint32_t* value, uint32_t comapre, uint32_t exchange);

}
