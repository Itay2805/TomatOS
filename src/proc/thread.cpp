#include <arch/gdt.hpp>
#include <util/critical_section.hpp>
#include "thread.hpp"
#include "process.hpp"

namespace proc {

    thread::thread(std::shared_ptr<process> parent)
        : parent(std::weak_ptr(parent))
        , tlock()
        , state(thread_state::WAITING)
        , context()
        , kernel(parent.get() == proc::kernel.get())
        , _tid(parent.get()->next_tid++)
    {

    }

    thread::~thread() = default;

    void thread::save_context(const intr::interrupt_context& context) {
        util::critical_section cs(this->lock());

        this->context.r15 = context.r15;
        this->context.r14 = context.r14;
        this->context.r13 = context.r13;
        this->context.r12 = context.r12;
        this->context.r11 = context.r11;
        this->context.r10 = context.r10;
        this->context.r9 = context.r9;
        this->context.r8 = context.r8;
        this->context.rbp = context.rbp;
        this->context.rdi = context.rdi;
        this->context.rsi = context.rsi;
        this->context.rdx = context.rdx;
        this->context.rcx = context.rcx;
        this->context.rbx = context.rbx;
        this->context.rax = context.rax;
        this->context.rip = context.rip;
        this->context.rsp = context.rsp;
        this->context.rflags = context.rflags;
    }

    void thread::load_context(intr::interrupt_context& context) {
        util::critical_section cs(this->lock());

        // set the segment registers
        if(kernel) {
            context.cs = arch::gdt::kernel_code;
            context.ds = arch::gdt::kernel_data;
            context.ss = arch::gdt::kernel_data;
        }else {
            context.cs = arch::gdt::user_code;
            context.ds = arch::gdt::user_data;
            context.ss = arch::gdt::user_data;
        }

        // copy the flags
        context.rflags = this->context.rflags;
        context.r15 = this->context.r15;
        context.r14 = this->context.r14;
        context.r13 = this->context.r13;
        context.r12 = this->context.r12;
        context.r11 = this->context.r11;
        context.r10 = this->context.r10;
        context.r9 = this->context.r9;
        context.r8 = this->context.r8;
        context.rbp = this->context.rbp;
        context.rdi = this->context.rdi;
        context.rsi = this->context.rsi;
        context.rdx = this->context.rdx;
        context.rcx = this->context.rcx;
        context.rbx = this->context.rbx;
        context.rax = this->context.rax;
        context.rip = this->context.rip;
        context.rsp = this->context.rsp;

        // set the page table
        auto proc = this->parent.lock();
        ASSERT(proc);
        proc->vmm()->switch_to_context();
    }

    void thread::kill() {
        this->state = thread_state::DEAD;

        // TODO: dequeue from scheduler

        // remove the thread from the process
        auto proc = this->parent.lock();
        ASSERT(proc);
        {
            util::critical_section cs(proc->lock());
            proc->threads.remove(this->_tid);
        }

        //******************************
        // DO NOT RUN CODE AFTER THIS
        //
        // Because we are deleting what
        // that might be the only reference
        // to the thread, we are probably
        // deleted and the memory of us
        // is invalid
        //
        //******************************
    }

}
