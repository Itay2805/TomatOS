BITS 64

EXTERN syscall_handler
EXTERN kernel_stack

GLOBAL syscall_handler_stub
syscall_handler_stub:
    ; save the return and rflags
    ;push rsp
    ;mov rsp, kernel_stack
    
    ;push rcx
    ;push r11
    ;mov rdi, rcx
    ; call the actual handler
    ;call syscall_handler

    ; restore the rip and rflags
    ;pop r11
    ;pop rcx

    ;pop rsp

    sysret