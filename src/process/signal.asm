BITS 64
SECTION .user.text

GLOBAL signal_return
signal_return:
    pop r11
    pop r10
    pop r9
    pop r8
    pop rcx
    pop rdx
    pop rsi
    pop rdi
    pop rax
    ret