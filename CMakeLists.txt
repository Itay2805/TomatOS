cmake_minimum_required(VERSION 3.10)

project(TomatOS)

# Setup C
set(CMAKE_C_COMPILER /usr/bin/clang-9)

# Setup using asm
enable_language(ASM)
set(CMAKE_ASM_COMPILER /usr/bin/nasm)
set(CMAKE_ASM_COMPILE_OBJECT "<CMAKE_ASM_COMPILER> <SOURCE> <FLAGS> -o <OBJECT>")

# Setup compilation flags
set(CMAKE_C_FLAGS "-target x86_64-unknown-elf -Wall -mno-sse -mno-sse2 -mno-mmx -mno-80387 -ffreestanding -nostdlib -nostdinc -mno-red-zone -mcmodel=kernel -Wno-unused-label -fno-pie -static -g -Werror")
set(CMAKE_EXE_LINKER_FLAGS "--oformat elf_amd64 --Bstatic --nostdlib -T config/linker.ld")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__FILENAME__='\"$(subst ${CMAKE_SOURCE_DIR}/,,$(abspath $<))\"'")
file(GLOB_RECURSE SOURCE_FILES ./*.c ./*.h)

include_directories(lib/libc)
include_directories(lib/stb)
include_directories(lib/)
include_directories(src/)

add_executable(TomatOS ${SOURCE_FILES})
