#include <libc/stddef.h>
#include <libc/string.h>
#include <memory/vmm.h>
#include "smp.h"

extern char smp_trampoline[];
extern uint64_t smp_trampoline_size;

void smp_startup() {
    debug_log("[*] Preparing SMP\n");

    // start by copying the trampoline shellcode
    memcpy(PHYSICAL_TO_DIRECT((void*)0x1000), smp_trampoline, smp_trampoline_size);

}
