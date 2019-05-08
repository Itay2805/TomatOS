cmake_minimum_required(VERSION 3.10)

project(TomatKernel)

# Setup paths
set(KERNEL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/kernel")

# Setup C
set(CMAKE_C_COMPILER /usr/bin/gcc)

# Setup using asm
enable_language(ASM)
set(CMAKE_ASM_COMPILER /usr/bin/nasm)
set(CMAKE_ASM_COMPILE_OBJECT "<CMAKE_ASM_COMPILER> <SOURCE> <FLAGS> -o <OBJECT>")

# Setup compilation flags
set(CMAKE_C_FLAGS "-z max-page-size=4096 -fPIC -Wall -std=gnu99 -nostdinc -fno-builtin -fno-stack-protector -mno-sse -mno-red-zone  -march=x86-64")
set(CMAKE_ASM_FLAGS "-f elf64")
set(CMAKE_EXE_LINKER_FLAGS "-z max-page-size=4096 -no-pie -nostdlib -nodefaultlibs -nostartfiles -T ${KERNEL_PATH}/linker.ld")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__FILENAME__='\"$(subst ${CMAKE_SOURCE_DIR}/,,$(abspath $<))\"'")
# For debugging
if (CMAKE_BUILD_TYPE MATCHES Debug)
    set(CMAKE_C_FLAGS "-g ${CMAKE_C_FLAGS}")
    set(CMAKE_ASM_FLAGS "-g ${CMAKE_ASM_FLAGS}")
endif ()

# Setup kernel sources
set(KERNEL_SOURCE_FILES
        kernel/common/common.h
        kernel/common/kernel_info.h
        kernel/common/multiboot.h
        kernel/common/stdbool.h
        kernel/common/stdint.h
        kernel/common/stdarg.h
        kernel/common/string.c
        kernel/common/string.h
        kernel/common/buf.c
        kernel/common/buf.h
        kernel/common/map.c
        kernel/common/map.h

        kernel/common/mini-printf.c
        kernel/common/mini-printf.h
        kernel/common/except.c
        kernel/common/except.h

        kernel/graphics/font.c
        kernel/graphics/font.h
        kernel/graphics/term.c
        kernel/graphics/term.h

        kernel/memory/pmm.c
        kernel/memory/pmm.h
        kernel/memory/vmm.c
        kernel/memory/vmm.h
        kernel/memory/mm.c
        kernel/memory/mm.h
        kernel/memory/gdt.h

        kernel/interrupts/irq.c
        kernel/interrupts/irq.asm
        kernel/interrupts/irq.h
        kernel/interrupts/isr.c
        kernel/interrupts/isr.asm
        kernel/interrupts/isr.h
        kernel/interrupts/idt.c
        kernel/interrupts/idt.h
        kernel/interrupts/interrupts.h
        kernel/interrupts/interrupts.asm

        kernel/drivers/portio.h
        kernel/drivers/pic.c
        kernel/drivers/pic.h
        kernel/drivers/pic.asm
        kernel/drivers/pit.c
        kernel/drivers/pit.h

        kernel/process/thread.c
        kernel/process/thread.h
        kernel/process/process.c
        kernel/process/process.h
        kernel/process/scheduler.c
        kernel/process/scheduler.h
        kernel/process/syscall.asm
        kernel/process/syscall.c
        kernel/process/syscall.h

        kernel/locks/spinlock.h
        kernel/locks/rwlock.h


        kernel/cpu/rflags.h
        kernel/cpu/msr.h
        kernel/cpu/cpuid.h
        kernel/cpu/control.h

        kernel/resource/resource_manager.asm
        kernel/resource/resource_manager.c
        kernel/resource/resource_manager.h
        kernel/resource/resource.c
        kernel/resource/resource.h

        kernel/providers/zero/zero_provider.h
        kernel/providers/zero/zero_provider.c

        kernel/multiboot.asm
        kernel/kernel.c)

include_directories(${KERNEL_PATH})
add_executable(TomatKernel "${KERNEL_SOURCE_FILES}")
add_custom_command(
        TARGET TomatKernel
        COMMAND mkdir -p ${CMAKE_BINARY_DIR}/iso/boot/grub
        COMMAND cp ${CMAKE_BINARY_DIR}/TomatKernel ${CMAKE_BINARY_DIR}/iso/boot/TomatKernel
        COMMAND cp ${CMAKE_SOURCE_DIR}/grub.cfg ${CMAKE_BINARY_DIR}/iso/boot/grub/grub.cfg
        COMMAND grub-mkrescue -o ${CMAKE_SOURCE_DIR}/tomatos.iso ${CMAKE_BINARY_DIR}/iso
)
