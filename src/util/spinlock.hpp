#pragma once

#include <stdint.h>

namespace util {

    class spinlock_guard;

    class spinlock {
    private:

        enum lock_state : uint32_t {
            LOCK_RELEASED,
            LOCK_ACQUIRED
        };

        volatile uint32_t latch = LOCK_RELEASED;

    public:

        void acquire();

        bool acquire_or_fail();

        void release();

    };


    class spinlock_guard {
    private:
        spinlock& lock;

    public:

        explicit spinlock_guard(spinlock& lock)
            : lock(lock)
        {
            lock.acquire();
        }

        ~spinlock_guard() {
            lock.release();
        }

    };

}