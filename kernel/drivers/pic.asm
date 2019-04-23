GLOBAL pic_disable
pic_disable:
    push rbp
    mov rbp, rsp

    mov al, 0xff
    out 0xa1, al
    out 0x21, al

    mov rsp, rbp
    pop rbp
    ret