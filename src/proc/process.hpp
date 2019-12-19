#pragma once

#include <mem/vmm.hpp>

namespace proc {

    class process {
    private:
        mem::vmm::context vmm_context;

    public:



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
    void init_kernel_process(mem::vmm::context& context);

}
