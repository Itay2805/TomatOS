#pragma once

#include <mem/vmm.hpp>
#include <util/map.hpp>
#include <util/smarter.hpp>

namespace proc {

    // forward declare
    class thread;

    class process {
    private:
        _Alignas(alignof(mem::vmm::context)) uint8_t vmm_context_buffer[sizeof(mem::vmm::context)]{0};
        mem::vmm::context* vmm_context;

        util::map<int, util::shared_ptr<thread>> threads;

    public:

        explicit process(bool kernel = false);
        ~process();

        inline mem::vmm::context* vmm() { return this->vmm_context; }

    };

    /*
     * This is the kernel process
     * all kernel threads live in
     * this process
     */
    extern process* kernel;

    /**
     * this will initialize the kernel process
     */
    void init_kernel_process();

}
