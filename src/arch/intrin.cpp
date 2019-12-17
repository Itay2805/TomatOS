#include "intrin.hpp"

namespace arch {

    void cpu_pause() {
        asm volatile ("pause");
    }

    void cpu_sleep() {
        asm volatile ("hlt" ::: "memory");
    }

    void memory_fence() {
        asm volatile ("" ::: "memory");
    }

    void enable_interrupts() {
        asm volatile ("sti" ::: "memory");
    }

    void disable_interrupts() {
        asm volatile ("cli" ::: "memory");
    }

    bool get_interrupt_state() {
        IA32_EFLAGS eflags = read_eflags();
        return eflags.IF == 1;
    }

    void set_interrupt_state(bool state) {
        if(state) {
            enable_interrupts();
        }else {
            disable_interrupts();
        }
    }

    bool save_and_disable_interrupts() {
        bool state = get_interrupt_state();
        disable_interrupts();
        return state;
    }

    IA32_EFLAGS read_eflags() {
        IA32_EFLAGS eflags;
        asm volatile (
            "pushfq\n\t"
            "pop %0"
            : "=r" (eflags.raw)
        );
        return eflags;
    }

}

