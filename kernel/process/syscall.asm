BITS 64

EXTERN syscall_handler
EXTERN kernel_stack

SECTION .text

GLOBAL syscall_handler_stub
syscall_handler_stub:

    ; int num and error code, we just set these to 0 since these are not used in this handler
    push qword 0
    push qword 0

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
    mov rcx, rax
    mov rax, ds
    push rax
    mov ax, 16
    mov ds, ax
    mov es, ax
    mov gs, ax
    mov fs, ax
    mov ss, ax
    mov rax, rcx

    call syscall_handler

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

    ; remove the int num and error code
    add rsp, 16

    iretq

; GLOBAL syscall_handler_stub
; syscall_handler_stub:
;     ; save the return and rflags
;     mov [user_stack_save], rsp
;     mov rsp, kernel_stack
    
;     push rcx
;     push r11
;     mov rdi, rcx
    
;     ; call the actual handler
;     call syscall_handler

;     ; restore the rip and rflags
;     pop r11
;     pop rcx

;     mov rsp, [user_stack_save]

;     sysret

; SECTION .data
; ; TODO: Find a better solution for this (cpu local storage)
; user_stack_save:
;     dq 0