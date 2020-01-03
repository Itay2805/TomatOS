#include <arch/intrin.hpp>
#include <arch/sync.hpp>
#include "spinlock.hpp"

namespace util {

    void spinlock::lock() {
        arch::memory_fence();
        while(!this->lock_or_fail()) {
            arch::cpu_pause();
        }
    }

    bool spinlock::lock_or_fail() {
        arch::memory_fence();
        uint32_t result = arch::sync::interlocked_compare_exchange(&this->latch, LOCK_RELEASED, LOCK_ACQUIRED);
        arch::memory_fence();
        return result == LOCK_RELEASED;
    }

    void spinlock::unlock() {
        arch::memory_fence();
        this->latch = LOCK_RELEASED;
        arch::memory_fence();
    }

}
