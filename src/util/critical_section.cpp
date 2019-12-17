#include <arch/intrin.hpp>
#include "critical_section.hpp"

namespace util {

    critical_section::critical_section()
        : interrupt_state(arch::save_and_disable_interrupts())
        , lock()
        , guard(this->lock)
    {
    }

    critical_section::critical_section(spinlock& lock)
        : interrupt_state(arch::save_and_disable_interrupts())
        , guard(lock)
    {
    }

    critical_section::~critical_section() {
        arch::set_interrupt_state(this->interrupt_state);
    }

}
