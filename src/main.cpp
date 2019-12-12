#include <mem/pmm.hpp>

#include <assert.h>
#include <stddef.h>
#include <tboot.h>

extern "C" void kernel_main(uint64_t magic, tboot_info_t* info) {
    debug_log("[+] Entered kernel!\n");

    mem::pmm::init(info);

    ASSERT(false);
}
