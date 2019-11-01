#include <stdint.h>
#include <stdbool.h>

#include <memory/pmm.h>
#include <util/debug.h>
#include <tboot.h>

void kernel_main(uint32_t magic, tboot_info_t* info) {
    debug_log("[+] Entered kernel\n");

    pmm_init(info);

    ASSERT(false);
}
