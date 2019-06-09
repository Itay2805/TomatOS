BITS 64

EXTERN irq_common

GLOBAL irq_0
GLOBAL irq_1
GLOBAL irq_2
GLOBAL irq_3
GLOBAL irq_4
GLOBAL irq_5
GLOBAL irq_6
GLOBAL irq_7
GLOBAL irq_8
GLOBAL irq_9
GLOBAL irq_10
GLOBAL irq_11
GLOBAL irq_12
GLOBAL irq_13
GLOBAL irq_14
GLOBAL irq_15

irq_common_stub:
    push qword 0 ; cr3 (set by handler)

    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    ; store ds
    mov rax, ds
    push rax
    mov ax, 16
    mov ds, ax
    mov es, ax
    mov gs, ax
    mov fs, ax
    mov ss, ax

    call irq_common

    ; restore ds
    pop rax
    mov ds, ax
    mov es, ax
    mov gs, ax
    mov fs, ax

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    ; ignore the int and error code
    add rsp, 8*3

    iretq


irq_0:
    push qword 0 ; error_code
    push qword 0 ; int_num
    jmp irq_common_stub

irq_1:
    push qword 0 ; error_code
    push qword 1 ; int_num
    jmp irq_common_stub

irq_2:
    push qword 0 ; error_code
    push qword 2 ; int_num
    jmp irq_common_stub

irq_3:
    push qword 0 ; error_code
    push qword 3 ; int_num
    jmp irq_common_stub

irq_4:
    push qword 0 ; error_code
    push qword 4 ; int_num
    jmp irq_common_stub

irq_5:
    push qword 0 ; error_code
    push qword 5 ; int_num
    jmp irq_common_stub

irq_6:
    push qword 0 ; error_code
    push qword 6 ; int_num
    jmp irq_common_stub

irq_7:
    push qword 0 ; error_code
    push qword 7 ; int_num
    jmp irq_common_stub

irq_8:
    push qword 0 ; error_code
    push qword 8 ; int_num
    jmp irq_common_stub

irq_9:
    push qword 0 ; error_code
    push qword 9 ; int_num
    jmp irq_common_stub

irq_10:
    push qword 0 ; error_code
    push qword 10 ; int_num
    jmp irq_common_stub

irq_11:
    push qword 0 ; error_code
    push qword 11 ; int_num
    jmp irq_common_stub

irq_12:
    push qword 0 ; error_code
    push qword 12 ; int_num
    jmp irq_common_stub

irq_13:
    push qword 0 ; error_code
    push qword 13 ; int_num
    jmp irq_common_stub

irq_14:
    push qword 0 ; error_code
    push qword 14 ; int_num
    jmp irq_common_stub

irq_15:
    push qword 0 ; error_code
    push qword 15 ; int_num
    jmp irq_common_stub
