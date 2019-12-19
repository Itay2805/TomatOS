#include <util/cpp_runtime.h>
#include "process.hpp"

namespace proc {


    alignas(alignof(process)) uint8_t kernel_process_buffer[sizeof(process)];
    process* kernel;

    void init_kernel_process() {
        kernel = new (kernel_process_buffer) process();
    }

}
