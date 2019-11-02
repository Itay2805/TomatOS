#include <libc/stddef.h>
#include "sync.h"
#include "arch.h"
#include "debug.h"

#define SPIN_LOCK_RELEASED          (0)
#define SPIN_LOCK_ACQUIRED          (1)

void aquire_lock(lock_t* lock) {
    while(!aquire_lock_or_fail(lock)) {
        cpu_pause();
    }
}

bool aquire_lock_or_fail(lock_t* lock) {
    ASSERT(lock != NULL);
    return interlocked_compare_exchange(lock, SPIN_LOCK_RELEASED, SPIN_LOCK_ACQUIRED) == SPIN_LOCK_ACQUIRED;
}

void release_lock(lock_t* lock) {
    *lock = SPIN_LOCK_RELEASED;
}

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
        "lock                 \n\t"
        "cmpxchgl    %2, %1   \n\t"
        : "+a" (comapre),      // %0
          "+m" (*value)             // %1
        : "r"  (exchange)      // %2
        : "memory", "cc"
    );
    return comapre;
}
