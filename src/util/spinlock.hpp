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

        _Atomic uint32_t latch = LOCK_RELEASED;

    public:

        void lock();

        bool lock_or_fail();

        void unlock();

        spinlock& operator=(const spinlock&) = delete;
        spinlock(const spinlock&) = delete;
        spinlock() = default;

    };


    class spinlock_guard {
    private:
        spinlock& lock;

    public:

        explicit spinlock_guard(spinlock& lock)
            : lock(lock)
        {
            lock.lock();
        }

        ~spinlock_guard() {
            lock.unlock();
        }

        spinlock_guard& operator=(const spinlock_guard&) = delete;
        spinlock_guard(const spinlock_guard&) = delete;

    };

}