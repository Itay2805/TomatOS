#include "cpuid.hpp"

namespace arch {

    void cpuid(cpuid_function code, int subcode, uint32_t data[4]) {
        asm volatile("cpuid"
        : "=a"(data[CPUID_EAX])
        , "=b"(data[CPUID_EBX])
        , "=c"(data[CPUID_ECX])
        , "=d"(data[CPUID_EDX])

        : "a"(code)
        , "c"(subcode)
        );
    }

}