SECTION .rodata

GLOBAL smp_trampoline
smp_trampoline:
incbin "build/src/smp/trampoline.bin"
smp_trampoline_end:

GLOBAL smp_trampoline_size
smp_trampoline_size:
dq smp_trampoline_end - smp_trampoline
