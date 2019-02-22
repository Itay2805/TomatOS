cmake_minimum_required(VERSION 3.12)

project(TomatKernel)

# Setup paths
set(KERNEL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/kernel")

# Setup C
set(CMAKE_C_COMPILER gcc)

# Setup using asm
enable_language(ASM)
set(CMAKE_ASM_COMPILER nasm)
set(CMAKE_ASM_COMPILE_OBJECT "<CMAKE_ASM_COMPILER> <SOURCE> <FLAGS> -o <OBJECT>")

# Setup compilation flags
set(CMAKE_C_FLAGS "-fPIC -Wall -Wextra -std=gnu99 -nostdinc -fno-builtin -fno-stack-protector -march=x86-64")
set(CMAKE_ASM_FLAGS "-f elf64")
set(CMAKE_EXE_LINKER_FLAGS "-no-pie -nostdlib -nodefaultlibs -nostartfiles -T ${KERNEL_PATH}/linker.ld")

# For debugging
if (CMAKE_BUILD_TYPE MATCHES Debug)
    set(CMAKE_C_FLAGS "-g ${CMAKE_C_FLAGS}")
    set(CMAKE_ASM_FLAGS "-g ${CMAKE_ASM_FLAGS}")
endif ()

# Setup kernel sources
set(KERNEL_SOURCE_FILES
        kernel/klibc/stdint.h

        kernel/memory/virtual_memory_manager.c
        kernel/memory/virtual_memory_manager.h

        kernel/multiboot.asm
        kernel/kernel.c
        )

include_directories(${KERNEL_PATH})
add_executable(TomatKernel "${KERNEL_SOURCE_FILES}")