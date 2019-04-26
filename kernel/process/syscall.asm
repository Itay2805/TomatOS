BITS 64

EXTERN syscall_handler
EXTERN kernel_stack

SECTION .text

GLOBAL syscall_handler_stub
syscall_handler_stub:
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