BITS 32

; ====================================
;   Multiboot
; ====================================
;
; This is a multiboot one header
;
; ====================================


; ------------------------------------
; Header values
; ------------------------------------
MAGIC equ 0x1badb002
FLAGS equ ((1 << 0) | (1 << 1) | (1 << 2))
CHECKSUM equ -(MAGIC + FLAGS)

; ------------------------------------
; Actual header
; ------------------------------------
SECTION .multiboot_header
ALIGN 8
multiboot_header:
dd MAGIC
dd FLAGS
dd CHECKSUM

; these are the address fields, we just ignore these
dd 0 ; header_addr
dd 0 ; load_addr
dd 0 ; load_end_addr
dd 0 ; bss_end_addr
dd 0 ; entry_addr

; these are the graphics fields
dd 0    ; video mode
dd 1024 ; width
dd 768  ; height
dd 32   ; depth (bpp)

; ====================================
;   Multiboot entry
; ====================================
SECTION .text

global multiboot_main
multiboot_main:

    ; TODO: Check that we were loaded from multiboot

    ; TODO: Check we have PSE & long mode support

    ; set the first pml4 entry
    ; sets the address of the pdpe
    ; and then set some flags
     ; PRESENT | READ_WRITE
    mov eax, boot_pdpe
    or eax, 0x3
    mov [boot_pml4], eax

    ; set the pdpe entry, starts at physical 0
    ; all the way to 1GB
    ; PRESENT | READ_WRITE | LARGE_PAGE
	mov dword [boot_pdpe], 0x83

    ; setup the TSS entry in the gdt
    mov eax, tss64_end - tss64_start
    mov edx, tss64_start
    mov word [gdt64_start.tss], ax
    sar eax, 16
    and eax, 0xF
    or byte [gdt64_start.tss + 6], al
    mov eax, edx
    mov word [gdt64_start.tss + 2], dx
    sar eax, 16
    shr edx, 24
    mov byte [gdt64_start.tss + 4], al
    mov byte [gdt64_start.tss + 7], dl

    ; set the pml4
    mov eax, boot_pml4
    mov cr3, eax

    ; Enable PAE-paging
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; Enter long mode compatability mode
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    ; Enable paging
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    ; Jump to the kernel using real long mode (64bit)
    lgdt [gdt64]

    mov ax, 0x2b
    ltr ax

    mov ax, 16
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 8:multiboot_64bit

BITS 64
    extern kernel_main
    multiboot_64bit:
        mov rsp, kernel_stack
        mov rbp, kernel_stack
        mov rdi, rbx
        call kernel_main

        ; halt if we reach here
        cli
        hlt


; ====================================
;   64Bit GDT
; ====================================
SECTION .data

; --------------------------------
; The GDT64 table
; --------------------------------
gdt64_start:

    ; --------------------------------
    ; NULL entry
    ; --------------------------------
    dw 0x0000
    dw 0x0000
    db 0x00
    db 00000000b
    db 00000000b
    db 0x00

    ; --------------------------------
    ; Kernel code segment (8)
    ; --------------------------------
    dw 0x0000
    dw 0x0000
    db 0x00
    db 10011010b
    db 00100000b
    db 0x00

    ; --------------------------------
    ; Kernel data segment (16)
    ; --------------------------------
    dw 0x0000
    dw 0x0000
    db 0x00
    db 10010010b
    db 00000000b
    db 0x00

    ; --------------------------------
    ; User data segment (24)
    ; --------------------------------
    dw 0x0000
    dw 0x0000
    db 0x00
    db 11110010b
    db 00000000b
    db 0x00

    ; --------------------------------
    ; User code segment (32)
    ; --------------------------------
    dw 0x0000
    dw 0x0000
    db 0x00
    db 11111010b
    db 00100000b
    db 0x00

    ; --------------------------------
    ; TSS (40)
    ; --------------------------------
    ;dw (tss64_end - tss64) & 0xFFFF
    ;dw tss64 
    ;db (tss64 >> 16) & 0xFF
    ;db 10001001b
    ;db 11000000b | (((tss64_end - tss64) >> 16) & 0xF)
    ;db (tss64 >> 24) & 0xFF
    .tss:
    dw 0
    dw 0
    db 0
    db 10001001b
    db 11000000b
    db 0
    dd 0
    dd 0


gdt64_end:

; --------------------------------
; The actual pointer
; --------------------------------
global gdt64
gdt64:
    dw gdt64_end - gdt64_start - 1
    dq gdt64_start

; ====================================
;   TSS 
; ====================================

tss64_start:
    dd 0
    ; Ring 0 RSP
    dq kernel_stack
    ; Ring 1 RSP
    dq kernel_stack
    ; Ring 2 RSP
    dq kernel_stack
    dq 0
    ; IST1-7
    dq kernel_stack
    dq kernel_stack
    dq kernel_stack
    dq kernel_stack
    dq kernel_stack
    dq kernel_stack
    dq kernel_stack
    dq 0
    ; IOPB offset
    dd (tss64_iopb - tss64_start) << 16
tss64_end:

; --------------------------------
; The IOPB of the TSS, just have it empty
; --------------------------------
tss64_iopb:
    dq 0

; ====================================
;   Paging (1GB identity mapping)
; ====================================
SECTION .bss

alignb 4096
global boot_pml4
boot_pml4:
resq 512

alignb 4096
global boot_pdpe
boot_pdpe:
resq 512

; ====================================
;   Stack
; ====================================
SECTION .bss
GLOBAL kernel_stack

resb 2 * 1024 * 1024
kernel_stack: