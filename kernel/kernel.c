#include "kernel.h"

#include "graphics/term.h"

extern uint64_t* boot_pdpe;


void kernel_main(multiboot_info_t* info) {
    // first of all map the first 4GB
    for(int i = 0; i < 4; i++) {
        boot_pdpe[i] = ((uint64_t)((0x7FFFFFFFC0000000) & (i * (1 << 20)))) | (1 << 1) | (1 << 0) | (1 << 7);
    }

    term_init(info);
    term_clear();

    term_write("We are in kernel_main!");

    for(;;) {
        asm("hlt");
    }
}
