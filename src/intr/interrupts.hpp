#pragma once

#include <arch/intrin.hpp>
#include <stdint.h>

namespace intr {

    struct __attribute__((packed)) interrupt_context {
        // the data segment to return to
        uint64_t ds;

        // general purpose registers
        uint64_t r15;
        uint64_t r14;
        uint64_t r13;
        uint64_t r12;
        uint64_t r11;
        uint64_t r10;
        uint64_t r9;
        uint64_t r8;
        uint64_t rbp;
        uint64_t rdi;
        uint64_t rsi;
        uint64_t rdx;
        uint64_t rcx;
        uint64_t rbx;
        uint64_t rax;

        // the interrupt number
        uint64_t int_num;

        // error code (if any)
        uint64_t error_code;

        // the rip (set this to return to another place)
        uint64_t rip;

        // the code segment to return to
        uint64_t cs;

        // the rflags
        arch::IA32_EFLAGS rflags;

        // the stack pointer and segment
        uint64_t rsp;
        uint64_t ss;
    };

}
