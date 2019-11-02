#include <stdint.h>
#include <stdbool.h>

#include <memory/pmm.h>
#include <util/debug.h>
#include <tboot.h>
#include <memory/vmm.h>
#include <smp/smp.h>

void kernel_main(uint32_t magic, tboot_info_t* info) {
    debug_log("[+] Entered kernel!\n");

    // we can init the idt right away
    idt_init();

    // do memory initialization
    pmm_init(info);
    vmm_init(info);
    pmm_post_vmm();

    smp_startup();

    ASSERT(false);
}
