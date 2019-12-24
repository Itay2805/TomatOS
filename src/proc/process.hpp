#pragma once

#include <mem/vmm.hpp>
#include <util/map.hpp>
#include <util/smarter.hpp>

namespace proc {

    // forward declare
    class thread;

    class process {
    private:
        util::spinlock _lock;
        int _pid;

        _Alignas(alignof(mem::vmm::context)) uint8_t vmm_context_buffer[sizeof(mem::vmm::context)]{0};
        mem::vmm::context* vmm_context;

        util::map<int, util::shared_ptr<thread>> threads;
        int next_tid = 1;

        friend thread;

    public:

        explicit process(bool kernel = false);
        ~process();

        /*
         * Will create a new thread for this process
         */
        util::shared_ptr<thread> create_thread();

        /**
         * Get the VMM context of the process
         */
        inline mem::vmm::context* vmm() { return this->vmm_context; }

        /**
         * Get the process lock
         */
        inline util::spinlock& lock() { return this->_lock; }

        /**
         * will kill all the threads of the process including the process
         */
        void kill();

        /*
         * Get the process pid
         */
        inline int pid() const { return this->_pid; }
    };

    /*
     * This is the kernel process
     * all kernel threads live in
     * this process
     */
    extern util::shared_ptr<process> kernel;

    /*
     * lock related to anything to do with the processes map
     */
    extern util::spinlock lock;

    /*
     * The processes map, contains all the processes except of the kernel
     */
    extern util::map<int, util::shared_ptr<process>> processes;

    /**
     * Will create a new process
     */
    util::shared_ptr<process> create_process(bool kernel = false);

}
