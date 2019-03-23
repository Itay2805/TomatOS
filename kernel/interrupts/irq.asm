BITS 64

EXTERN irq_common

GLOBAL irq_pit
GLOBAL irq_keyboard
GLOBAL irq_cascade
GLOBAL irq_com2
GLOBAL irq_com1
GLOBAL irq_lpt2
GLOBAL irq_floppy
GLOBAL irq_lpt1
GLOBAL irq_cmos
GLOBAL irq_peripherals_1
GLOBAL irq_peripherals_2
GLOBAL irq_peripherals_3
GLOBAL irq_ps2_mouse
GLOBAL irq_coprocessor
GLOBAL irq_primary_ata
GLOBAL irq_secondary_ata

GLOBAL irq_common
irq_common_stub:
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    ; store ds
    xor rax, rax
    mov ax, ds
    push rax
    mov ax, 16
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call irq_common

    ; restore ds
    pop rax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    ; ignore the int and error code
    add rsp, 16

    mov rsp, rbp
    pop rbp
    sti
    iretq


irq_pit:
    cli
    push rbp
    mov rbp, rsp
    push qword 0
    push qword 32
    jmp irq_common_stub

irq_keyboard:
    cli
    push rbp
    mov rbp, rsp
    push qword 0
    push qword 33
    jmp irq_common_stub

irq_cascade:
    cli
    push rbp
    mov rbp, rsp
    push qword 0
    push qword 34
    jmp irq_common_stub

irq_com2:
    cli
    push rbp
    mov rbp, rsp
    push qword 0
    push qword 35
    jmp irq_common_stub

irq_com1:
    cli
    push rbp
    mov rbp, rsp
    push qword 0
    push qword 36
    jmp irq_common_stub

irq_lpt2:
    cli
    push rbp
    mov rbp, rsp
    push qword 0
    push qword 37
    jmp irq_common_stub

irq_floppy:
    cli
    push rbp
    mov rbp, rsp
    push qword 0
    push qword 38
    jmp irq_common_stub

irq_lpt1:
    cli
    push rbp
    mov rbp, rsp
    push qword 0
    push qword 39
    jmp irq_common_stub

irq_cmos:
    cli
    push rbp
    mov rbp, rsp
    push qword 0
    push qword 40
    jmp irq_common_stub

irq_peripherals_1:
    cli
    push rbp
    mov rbp, rsp
    push qword 0
    push qword 41
    jmp irq_common_stub

irq_peripherals_2:
    cli
    push rbp
    mov rbp, rsp
    push qword 0
    push qword 42
    jmp irq_common_stub

irq_peripherals_3:
    cli
    push rbp
    mov rbp, rsp
    push qword 0
    push qword 43
    jmp irq_common_stub

irq_ps2_mouse:
    cli
    push rbp
    mov rbp, rsp
    push qword 0
    push qword 44
    jmp irq_common_stub

irq_coprocessor:
    cli
    push rbp
    mov rbp, rsp
    push qword 0
    push qword 45
    jmp irq_common_stub

irq_primary_ata:
    cli
    push rbp
    mov rbp, rsp
    push qword 0
    push qword 46
    jmp irq_common_stub

irq_secondary_ata:
    cli
    push rbp
    mov rbp, rsp
    push qword 0
    push qword 47
    jmp irq_common_stub