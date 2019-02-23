#include "kernel.h"

void kernel_main(void) {
    for(;;) {
        asm("hlt");
    }
}
