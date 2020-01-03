#include <mem/pmm.hpp>
#include <mem/vmm.hpp>

#include <assert.h>
#include <stddef.h>
#include <tboot.h>
#include <proc/process.hpp>
#include <arch/idt.hpp>
#include <util/cpp_runtime.h>
#include <proc/thread.hpp>

extern "C" void kernel_main(uint64_t magic, tboot_info_t* info) {
    // init the logger and try to log something
    util::init_debug_logger();
    util::log() << "[+] Entered kernel!" << frg::endlog;

    // initialize interrupt handler
    arch::idt::init();

    // initialize memory related stuff
    mem::pmm::init(info);
    mem::vmm::init(info);
    mem::pmm::post_vmm_init();

    // run the constructors (should be safe now
    // that we have the vmm initialized)
    util::call_ctors();

    util::log() << "Hello World!" << frg::endlog;

    // create the kernel process
    proc::kernel = proc::create_process(true);
    auto thread = proc::kernel->create_thread();

    ASSERT(false);
}
