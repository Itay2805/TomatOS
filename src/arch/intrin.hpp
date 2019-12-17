#pragma once

#include <stdint.h>

namespace arch {

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // general cpu operations
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * Requests CPU to pause for a short period of time. Typically used in MP
     * systems to prevent memory starvation while waiting for a spin lock.
     */
    void cpu_pause();

    /**
     * Places the CPU in a sleep state until an interrupt is received. If interrupts
     * are disabled prior to calling this function, then the CPU will be placed in a
     * sleep state indefinitely.
     */
    void cpu_sleep();

    /**
     * All loads and stores that proceed calls to this function are guaranteed to be
     * globally visible when this function returns.
     */
    void memory_fence();

    /**
     * Enables CPU interrupts.
     */
    void enable_interrupts();

    /**
     * Disables CPU interrupts.
     */
    void disable_interrupts();

    /**
     * Returns true is interrupts are currently enabled. Otherwise
     * returns false.
     */
    bool get_interrupt_state();

    /**
     * Sets the current CPU interrupt state to the state specified by
     * state. If state is true, then interrupts are enabled. If
     * state is false, then interrupts are disabled. InterruptState is
     * returned.
     */
    void set_interrupt_state(bool state);

    /**
     * Disables CPU interrupts and returns the interrupt state prior to the disable
     * operation.
     */
    bool save_and_disable_interrupts();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // x86 specific stuff
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    union IA32_EFLAGS {
        struct {
            uint64_t CF:1;           ///< Carry Flag.
            uint64_t _reserved0:1;   ///< Reserved.
            uint64_t PF:1;           ///< Parity Flag.
            uint64_t _reserved1:1;   ///< Reserved.
            uint64_t AF:1;           ///< Auxiliary Carry Flag.
            uint64_t _reserved2:1;   ///< Reserved.
            uint64_t ZF:1;           ///< Zero Flag.
            uint64_t SF:1;           ///< Sign Flag.
            uint64_t TF:1;           ///< Trap Flag.
            uint64_t IF:1;           ///< Interrupt Enable Flag.
            uint64_t DF:1;           ///< Direction Flag.
            uint64_t OF:1;           ///< Overflow Flag.
            uint64_t IOPL:2;         ///< I/O Privilege Level.
            uint64_t NT:1;           ///< Nested Task.
            uint64_t _reserved3:1;   ///< Reserved.
            uint64_t RF:1;           ///< Resume Flag.
            uint64_t VM:1;           ///< Virtual 8086 Mode.
            uint64_t AC:1;           ///< Alignment Check.
            uint64_t VIF:1;          ///< Virtual Interrupt Flag.
            uint64_t VIP:1;          ///< Virtual Interrupt Pending.
            uint64_t ID:1;           ///< ID Flag.
            uint64_t _reserved4:10;  ///< Reserved.
        };
        uint64_t raw;
    };
    static_assert(sizeof(uint64_t) == sizeof(IA32_EFLAGS));

    /**
     * Read the eflags register
     */
    IA32_EFLAGS read_eflags();

}
