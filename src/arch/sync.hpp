#pragma once

#include <stdint.h>

namespace arch::sync {

    uint32_t interlocked_increment(volatile uint32_t* value);
    uint32_t interlocked_decrement(volatile uint32_t* value);
    uint32_t interlocked_compare_exchange(volatile uint32_t* value, uint32_t comapre, uint32_t exchange);

}
