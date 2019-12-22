#include <util/cpp_runtime.h>
#include <string.h>
#include "process.hpp"

namespace proc {

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // process class
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    process::process(bool kproc) {
        if(kproc) {
            this->vmm_context = mem::vmm::kernel;
        }else {
            this->vmm_context = new (vmm_context_buffer) mem::vmm::context();
        }
    }

    process::~process() {
        // make sure this is at the same place
        ASSERT(this->vmm_context == reinterpret_cast<mem::vmm::context*>(this->vmm_context_buffer));
        vmm_context->~context();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // kernel process related
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    alignas(alignof(process)) uint8_t kernel_process_buffer[sizeof(process)];
    process* kernel;

    void init_kernel_process() {
        kernel = new (kernel_process_buffer) process(true);
    }

}
