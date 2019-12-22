#pragma once

namespace arch::gdt {

    constexpr int kernel_code = 8;
    constexpr int kernel_data = 16;
    constexpr int user_data = (24 | 3);
    constexpr int user_code = (32 | 3);

}
