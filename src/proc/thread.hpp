#pragma once

#include <util/smarter.hpp>
#include <util/spinlock.hpp>
#include <util/list_entry.hpp>
#include <intr/interrupts.hpp>

namespace proc {

    class process;

    struct thread_context {
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

        uint64_t rip;
        uint64_t rsp;

        arch::IA32_EFLAGS rflags;
    };

    enum class thread_state {
        RUNNING,
        NORMAL,
        WAITING,
        DEAD
    };

    class thread {
    private:
        util::weak_ptr<process> parent;
        util::spinlock tlock;
        thread_state state;

        thread_context context;
        bool kernel;
        int _tid;

    public:
        explicit thread(util::shared_ptr<process> parent);
        ~thread();

        /**
         * Get the lock for the thread
         * @return
         */
        util::spinlock& lock() { return tlock; }

        /**
         * Get the thread state
         */
        [[nodiscard]]
        inline thread_state get_state() const { return this->state; }

        [[nodiscard]]
        inline int tid() const { return this->_tid; }

        /**
         * Will save the thread context from the interrupt context
         */
        void save_context(const intr::interrupt_context& context);

        /**
         * Will load the thread context to the interrupt context
         */
        void load_context(intr::interrupt_context& context);

        /**
         * Will kill the thread
         */
        void kill();
    };

}
