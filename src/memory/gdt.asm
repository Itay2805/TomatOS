BITS 64
SECTION .text

; GDT stuff
CODE_SEGMENT equ 8
DATA_SEGMENT equ 16

fix_data_segment:
	mov ax, DATA_SEGMENT
	mov ds, ax
	mov es, ax
	mov gs, ax
	mov fs, ax
	ret


GLOBAL gdt_fix_segments
gdt_fix_segments:
	mov rax, rsp
	push qword DATA_SEGMENT
	push qword rax
	pushf
	push qword CODE_SEGMENT
	push qword fix_data_segment
	iretq
