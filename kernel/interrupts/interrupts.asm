BITS 64

GLOBAL lidt
lidt:
    lidt [rdi]
    ret

GLOBAL breakpoint
breakpoint:
    int 3
    ret