#pragma once

#include <stdint.h>

namespace arch::gdt {

    constexpr uint16_t kernel_code = 8u;
    constexpr uint16_t kernel_data = 16u;
    constexpr uint16_t user_data = (24u | 3u);
    constexpr uint16_t user_code = (32u | 3u);

}
