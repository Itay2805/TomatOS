#include "sync.hpp"

namespace arch::sync {

    uint32_t interlocked_increment(volatile uint32_t* value) {
        uint32_t  result;
        __asm__ __volatile__ (
        "movl    $1, %%eax  \n\t"
        "lock               \n\t"
        "xadd    %%eax, %1  \n\t"
        "inc     %%eax      \n\t"
        : "=&a" (result),
        "+m" (*value)
        :
        : "memory", "cc"
        );
        return result;
    }

    uint32_t interlocked_decrement(volatile uint32_t* value) {
        uint32_t  result;
        __asm__ __volatile__ (
        "movl    $1, %%eax  \n\t"
        "lock               \n\t"
        "xadd    %%eax, %1  \n\t"
        "dec     %%eax      \n\t"
        : "=&a" (result),
        "+m" (*value)
        :
        : "memory", "cc"
        );
        return result;
    }

    uint32_t interlocked_compare_exchange(volatile uint32_t* value, uint32_t comapre, uint32_t exchange) {
        __asm__ __volatile__ (
        "lock cmpxchgl %2, %1"
        : "+a" (comapre),
        "+m" (*value)
        : "r"  (exchange)
        : "memory", "cc"
        );
        return comapre;
    }

}
