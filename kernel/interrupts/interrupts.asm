BITS 64

GLOBAL sti
sti:
    sti
    ret

GLOBAL cli
cli:
    cli
    ret

GLOBAL lidt
lidt:
    lidt [rdi]
    ret

GLOBAL hlt
hlt:
    hlt
    ret