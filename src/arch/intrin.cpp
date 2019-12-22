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

    void write_cr0(uint64_t value) {
        asm volatile ( "mov %0, %%cr0" : : "r"(value) );
    }

    uint64_t read_cr0() {
        uint64_t val;
        asm volatile ( "mov %%cr0, %0" : "=r"(val) );
        return val;
    }

    void write_cr2(uint64_t value) {
        asm volatile ( "mov %0, %%cr2" : : "r"(value) );
    }

    uint64_t read_cr2() {
        uint64_t val;
        asm volatile ( "mov %%cr2, %0" : "=r"(val) );
        return val;
    }

    void write_cr3(uint64_t value) {
        asm volatile ( "mov %0, %%cr3" : : "r"(value) );
    }

    uint64_t read_cr3() {
        uint64_t val;
        asm volatile ( "mov %%cr3, %0" : "=r"(val) );
        return val;
    }

    void write_cr4(uint64_t value) {
        asm volatile ( "mov %0, %%cr4" : : "r"(value) );
    }

    uint64_t read_cr4() {
        uint64_t val;
        asm volatile ( "mov %%cr4, %0" : "=r"(val) );
        return val;
    }


    void read_idtr(IA32_IDT* idtr) {
        __asm__ __volatile__ (
        "sidt  %0"
        : "=m" (*idtr)
        );
    }

    void write_idtr(IA32_IDT* idtr) {
        __asm__ __volatile__ (
        "lidt %0"
        :
        : "m" (*idtr)
        );
    }

}

