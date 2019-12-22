#include <mem/pmm.hpp>
#include <mem/vmm.hpp>

#include <assert.h>
#include <stddef.h>
#include <tboot.h>
#include <proc/process.hpp>
#include <arch/idt.hpp>

extern "C" void kernel_main(uint64_t magic, tboot_info_t* info) {
    debug_log("[+] Entered kernel!\n");

    // initialize interrupt handler
    arch::idt::init();

    // initialize memory related stuff
    mem::pmm::init(info);
    mem::vmm::init(info);
    mem::pmm::post_vmm_init();

    // initialize the kernel process
    proc::init_kernel_process();

    ASSERT(false);
}
