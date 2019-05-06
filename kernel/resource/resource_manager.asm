
GLOBAL dispatch_resource_call_trampoline
dispatch_resource_call_trampoline:
    push r10 ; pid
    push r11 ; tid

    call rax
    
    pop rsi
    pop rdi
    mov rax, rcx

    ; SYSCALL_PROVIDER_HANDLER_FINISHED
    mov rax, 8
    int 0x80
