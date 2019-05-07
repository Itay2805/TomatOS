
GLOBAL dispatch_resource_call_trampoline
dispatch_resource_call_trampoline:
    ; TODO: Move this to thread local storage that only the kernel can access
    push rdi ; pid
    push rsi ; tid

    ; does a call to rax, but makes so we can clear out rax
    push rax
    xor rax, rax
    ret

    ; get the pid and tid
    pop rsi
    pop rdi
    mov rcx, rax

    ; call SYSCALL_PROVIDER_HANDLER_FINISHED
    mov rax, 8
    int 0x80