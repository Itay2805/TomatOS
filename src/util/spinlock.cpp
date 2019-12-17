#include <arch/intrin.hpp>
#include <arch/sync.hpp>
#include "spinlock.hpp"

namespace util {

    void spinlock::acquire() {
        arch::memory_fence();
        while(!this->acquire_or_fail()) {
            arch::cpu_pause();
        }
    }

    bool spinlock::acquire_or_fail() {
        arch::memory_fence();
        uint32_t result = arch::sync::interlocked_compare_exchange(&this->latch, LOCK_RELEASED, LOCK_ACQUIRED);
        arch::memory_fence();
        return result == LOCK_RELEASED;
    }

    void spinlock::release() {
        arch::memory_fence();
        this->latch = LOCK_RELEASED;
        arch::memory_fence();
    }

}
