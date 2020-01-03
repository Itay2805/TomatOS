# Options

# force the compiler
CC=clang-8
LD=ld.lld-8

# what loggers we want to support
LOGGERS ?= E9 TERM

.PHONY: all \
		clean \
		image \
		kernel \
		qemu

# build the kernel image by default
all: bin/tomatos.img

#########################
# Creating a VDI
#########################

vdi: bin/tomatos.vdi

bin/tomatos.vdi: bin/tomatos.img
	cd bin && vboxmanage convertfromraw --format VDI tomatos.img tomatos.vdi

#########################
# Creating an HDD
#########################

hdd: bin/tomatos.hdd

bin/tomatos.hdd: bin/tomatos.img
	cd bin && cp tomatos.img tomatos.hdd

#########################
# Compiling the kernel
#########################

# Real mode blobs, needed for some stuff
REALFILES += $(shell find src -type f -name '*.real')
BINS := $(REALFILES:%.real=build/%.bin)

# Sources, all c files basically
SRCS += $(shell find src -type f -name '*.c')
SRCS += $(shell find src -type f -name '*.cpp')
SRCS += $(shell find src -type f -name '*.asm')

# Include libraries
SRCS += $(shell find lib/libc -type f -name '*.c')

# Get all the objects and dirs
OBJS := $(SRCS:%=build/%.o)
OBJDIRS := $(dir $(OBJS) $(BINS))

# Set the flags
COMMON_CFLAGS += \
	-target x86_64-unknown-elf \
	-Wall \
	-mno-sse \
	-mno-sse2 \
	-mno-mmx \
	-mno-80387 \
	-ffreestanding \
	-nostdinc \
	-mno-red-zone \
	-mcmodel=kernel \
	-fno-pie \
	-static \
	-g

# Lai cflags
LAI_CFLAGS = \
    -Ilib/libc \
    -Ilib/lai/include \
    $(COMMON_CFLAGS)

# The kernel flags
KERNEL_CFLAGS = \
    $(COMMON_CFLAGS) \
    -Werror

# STB options
KERNEL_CFLAGS += -DSTB_SPRINTF_NOFLOAT

# Add kernel include dirs
KERNEL_CFLAGS += -Ilib/cxxshim/stage2/include
KERNEL_CFLAGS += -Ilib/frigg/include
KERNEL_CFLAGS += -Ilib/libsmarter/include
KERNEL_CFLAGS += -Ilib/libc
KERNEL_CFLAGS += -Ilib/
#KERNEL_CFLAGS += -Ilib/lai/include
KERNEL_CFLAGS += -Isrc/

# Set the loggers
KERNEL_CFLAGS += $(LOGGERS:%=-D%_LOGGER)

# The kernel cppflags
KERNEL_CPPFLAGS := \
    $(KERNEL_CFLAGS) \
    -std=c++17 \
    -fno-rtti \
    -fno-exceptions

# Set the linking flags
LDFLAGS += \
	--oformat elf_amd64 \
	--Bstatic \
	--nostdlib \
	-T config/linker.ld

NASMFLAGS += \
	-g \
	-F dwarf \
	-f elf64

kernel: bin/image/tomatos.elf

# Link everything
bin/image/tomatos.elf: $(OBJDIRS) $(BINS) $(OBJS)
	mkdir -p bin/image
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

# build lai c files
build/lib/lai/%.c.o: lib/lai/%.c
	$(CC) $(LAI_CFLAGS) -c -o $@ $<

# build real mode asm code
build/%.bin: %.real
	nasm -f bin -o $@ $<

# Compile kernel cpp files
build/%.cpp.o: %.cpp
	$(CC) $(KERNEL_CPPFLAGS) -D __FILENAME__="\"$<\"" -c -o $@ $<

# Compile kernel c files
build/%.c.o: %.c
	$(CC) $(KERNEL_CFLAGS) -D __FILENAME__="\"$<\"" -c -o $@ $<

# Compile kernel asm files
build/%.asm.o: %.asm
	nasm $(NASMFLAGS) -o $@ $<

# Make the build dir
build/%:
	mkdir -p $@

#########################
# Bootloader compilation
#########################

bin/image/EFI/BOOT/BOOTX64.EFI:
	$(MAKE) -C ./boot/
	mkdir -p bin/image/EFI/BOOT/
	cp boot/bin/BOOTX64.EFI bin/image/EFI/BOOT/

#########################
# Creating a bootable
# image
#########################

# Shortcut
image: bin/tomatos.img

# Create the image
bin/tomatos.img: \
		bin/image/EFI/BOOT/BOOTX64.EFI \
		bin/image/tomatos.elf \
		tools/echfs/echfs-utils
	cp ./config/tomatboot.cfg ./bin/image/
	cp ./boot/bin/BOOTX64.EFI ./bin/image/
	export PATH='${PATH}:${PWD}/tools/echfs'; cd bin && ../tools/image-builder/image-builder.py ../config/image.yaml

#########################
# Tools we need
#########################

tools/echfs/echfs-utils:
	$(MAKE) -C ./tools/echfs/ echfs-utils

#########################
# Running in QEMU
#########################

QEMU_PATH ?= qemu-system-x86_64
#QEMU_PATH = ~/Documents/coding/qemu/bin/debug/native/x86_64-softmmu/qemu-system-x86_64

# Run qemu
qemu: bin/tomatos.img
	$(QEMU_PATH) -drive if=pflash,format=raw,readonly,file=tools/OVMF.fd -drive file=bin/tomatos.img,media=disk,format=raw -no-reboot -no-shutdown -machine q35 $(QEMU_ARGS)

#		-netdev tap,id=mynet0,ifname=tap0,script=no,downscript=no \
#		-device rtl8139,netdev=mynet0 \


qemu-debug: tools/OVMF.fd $(TOMATBOOT_UEFI_DIR_BIN)/BOOTX64.EFI $(TOMATBOOT_SHUTDOWN_DIR_BIN)/shutdown.elf bin/image/tomatos.elf bin/image/kbootcfg.bin
	$(QEMU_PATH) -drive if=pflash,format=raw,readonly,file=tools/OVMF.fd -net none -drive file=fat:rw:bin/image,media=disk,format=raw -no-reboot -no-shutdown -machine q35 -s -S $(QEMU_ARGS)

# Get the bios
tools/OVMF.fd:
	mkdir -p tools
	cd tools && wget http://downloads.sourceforge.net/project/edk2/OVMF/OVMF-X64-r15214.zip
	cd tools && unzip OVMF-X64-r15214.zip OVMF.fd
	rm tools/OVMF-X64-r15214.zip

#########################
# Clean
#########################

clean:
	rm -rf build bin

clean-all:
	$(MAKE) -C ./boot/ clean
	rm -rf build bin
