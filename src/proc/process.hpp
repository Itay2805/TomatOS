#pragma once

#include <mem/vmm.hpp>

namespace proc {

    class process {
    private:
        _Alignas(alignof(mem::vmm::context)) uint8_t vmm_context_buffer[sizeof(mem::vmm::context)];
        mem::vmm::context* vmm_context;

    public:

        process(bool kernel = false);
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
