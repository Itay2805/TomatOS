SECTION .rodata

GLOBAL g_smp_trampoline
g_smp_trampoline:
incbin "kernel/arch/amd64/smp.bin"
g_smp_trampoline_end:

GLOBAL g_smp_trampoline_size
g_smp_trampoline_size:
dq g_smp_trampoline_end - g_smp_trampoline