#include <memory/vmm.h>
#include "rsdp.h"

rsdp_t* rsdp;
rsdp2_t* rsdp2;

void rsdp_init(tboot_info_t* info) {
    ASSERT(info->rsdp != NULL);

    rsdp = (rsdp_t*)PHYSICAL_TO_DIRECT(info->rsdp);
    if(rsdp->revision >= 2) {
        rsdp2 = (rsdp2_t*)rsdp;
    }

    debug_log("[+] \tRSDP - %p (Rev. %d)\n", info->rsdp, rsdp->revision);
}
