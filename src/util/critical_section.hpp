#pragma once

#include "spinlock.hpp"

namespace util {

    /**
     * A critical section makes sure that from the entry of the block to the end
     * of it only one thread will enter, this is safe to be used inside of interrupt
     * handlers or functions which may be called from interrupt handlers
     */
    class critical_section {
    private:
        bool interrupt_state;
        spinlock lock;
        spinlock_guard guard;

    public:

        critical_section();
        critical_section(spinlock& lock);
        ~critical_section();

    };

}
