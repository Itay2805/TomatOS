#include "kernel.h"

#include "graphics/term.h"

void kernel_main(multiboot_info_t* info) {
    term_init(info);
    term_clear();

    term_write("We are in kernel_main!");

    for(;;) {
        asm("hlt");
    }
}
