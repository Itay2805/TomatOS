#include "msr.hpp"

namespace arch {

    uint64_t read_msr(uint32_t code) {
        uint32_t low, high;
        asm volatile("rdmsr" : "=a"(low), "=d"(high) : "c"(code));
        return ((uint64_t)high << 32u) | (uint64_t)low;
    }

    void write_msr(uint32_t code, uint64_t value) {
        asm volatile("wrmsr" : : "c"(code), "a"(value & 0xFFFFFFFF), "d"(value >> 32));
    }

}
