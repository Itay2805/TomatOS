BITS 32

; ====================================
;   Multiboot
; ====================================
;
; This is a multiboot one header
; The main reason we support this is
; for easier booting in qemu
;
; ====================================


; ------------------------------------
; Header values
; ------------------------------------
MAGIC equ 0x1badb002
FLAGS equ ((1 << 0) | (1 << 1))
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
; ====================================
;   Multiboot entry
; ====================================
SECTION .text

global multiboot_main
multiboot_main:

    ; push the parameters to the
    ; kernel main function
    push eax
    push ebx

    ; set the first pml4 entry
    ; sets the address of the pdpe
    ; and then set some flags
     ; PRESENT | READ_WRITE
    mov eax, pdpe
    or eax, 0x3
    mov [pml4], eax

    ; set the pdpe entry, starts at physical 0
    ; all the way to 1GB
    ; PRESENT | READ_WRITE | LARGE_PAGE
    mov ecx, 0
    .mapping_loop:
        mov dword [2*ecx+pdpe], 0x83
        inc ecx
        cmp ecx, 512
        je .mapping_loop

    mov eax, pml4
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
    jmp 8:multiboot_64bit

BITS 64
    extern kernel_main
    multiboot_64bit:
        mov rsp, kernel_stack
        call kernel_main
        add rsp, 4

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
    dw 0xFFFF
    dw 0
    db 0
    db 0
    db 1
    db 0

    ; --------------------------------
    ; Code segment
    ; --------------------------------
    dw 0
    dw 0
    db 0
    db 10011010b
    db 10101111b
    db 0

    ; --------------------------------
    ; Data segment
    ; --------------------------------
    dw 0
    dw 0
    db 0
    db 10010010b
    db 00000000b
    db 0

gdt64_end:

; --------------------------------
; The actual pointer
; --------------------------------
global gdt64
gdt64:
    dw gdt64_end - gdt64_start - 1
    dq gdt64_start

; ====================================
;   Paging (1GB identity mapping)
; TODO: Do not actually include these in the binary
; ====================================
SECTION .bss

alignb 4096
pml4:
resq 512

alignb 4096
pdpe:
resq 512

; ====================================
;   Stack
; ====================================
SECTION .bss

resb 2 * 1024 * 1024
kernel_stack: