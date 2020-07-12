
CFLAGS += -target x86_64-unknown-elf
CFLAGS += -mno-sse -mno-sse2 -mno-mmx -mno-80387 -m64
CFLAGS += -mno-red-zone -mcmodel=kernel

CFLAGS += -D__TOMATOS_AMD64__

LDFLAGS += --oformat elf_amd64
LDFLAGS += -z max-page-size=0x1000
LDFLAGS += -T makefiles/amd64/linker.ld

SRCS += kernel/arch/amd64/debug.c
SRCS += kernel/arch/amd64/entry.c
SRCS += kernel/arch/amd64/gdt.c
SRCS += kernel/arch/amd64/intrin.c
SRCS += kernel/arch/amd64/vmm.c
