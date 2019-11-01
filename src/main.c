#include <stdint.h>
#include <stdbool.h>

#include <util/kernel_debugger.h>

void kernel_main(uint32_t magic, void* info) {
    debug_log("[+] Entered kernel\n");
    ASSERT(false);
}