BITS 64

EXTERN timer_interrupt_handler

GLOBAL timer_interrupt_stub
timer_interrupt_stub:
    push qword 0
    push qword 32

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

    call timer_interrupt_handler

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
    add rsp, 16

    iretq