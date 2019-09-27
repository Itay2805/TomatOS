
CC=clang-8
LD=ld.lld-8

.PHONY: all \
		clean \
		image \
		kernel \
		bootloader \
		qemu

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

# Sources, all c files basically
SRCS += $(shell find src -name '*.c')
SRCS += $(shell find src -name '*.asm')
SRCS += $(shell find lib -name '*.c')
OBJS := $(SRCS:%=build/%.o)
OBJDIRS := $(dir $(OBJS))

# include dirs
INCLUDE_DIRS += lib/
INCLUDE_DIRS += lib/libc
INCLUDE_DIRS += lib/common
INCLUDE_DIRS += lib/lai/include
INCLUDE_DIRS += src/

# Set the flags
CFLAGS += \
	-target x86_64-unknown-elf \
	-mno-sse \
	-ffreestanding \
	-nostdlib \
	-nostdinc \
	-fno-stack-check \
	-fno-stack-protector \
	-mno-red-zone \
	-mcmodel=kernel \
	-fno-omit-frame-pointer \
	-mno-omit-leaf-frame-pointer \
	-Wall \
	-fno-pie \
	-static \
	-flto \
	-Ofast \
	-g



# Set the include dirs
CFLAGS += $(INCLUDE_DIRS:%=-I%)

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

bin/image/tomatos.elf: $(OBJDIRS) $(OBJS)
	mkdir -p bin/image
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

build/%.c.o: %.c
	$(CC) $(CFLAGS) -D __FILENAME__="\"$<\"" -c -o $@ $<

build/%.asm.o: %.asm
	nasm $(NASMFLAGS) -o $@ $<

build/%:
	mkdir -p $@

#########################
# Bootloader compilation
#########################

# Set the bootloader stuff
TOMATBOOT_UEFI_DIR = boot/
TOMATBOOT_UEFI_DIR_BIN = bin/image/EFI/BOOT/

# Set the shutdown module stuff
TOMATBOOT_SHUTDOWN_DIR_BIN = bin/image/

# Include em'
include boot/tomatboot-uefi.mk

# Will build the bootloader image
# and then copy it to our space
bootloader: tomatboot-uefi-boot

#########################
# Creating a bootable
# image
#########################

# Shortcut
image: bin/tomatos.img

# Create the image
bin/tomatos.img: $(TOMATBOOT_UEFI_DIR_BIN)/BOOTX64.EFI $(TOMATBOOT_SHUTDOWN_DIR_BIN)/shutdown.elf bin/image/tomatos.elf bin/image/kbootcfg.bin tools/image-builder.py tools/tomatboot-config.py
	cd bin && ../tools/image-builder.py ../config/image.yaml

bin/image/kbootcfg.bin: config/boot.yaml tools/tomatboot-config.py
	./tools/tomatboot-config.py config/boot.yaml bin/image/kbootcfg.bin

#########################
# Tools we need
#########################

tools/image-builder.py:
	mkdir -p tools
	cd tools && wget https://raw.githubusercontent.com/TomatOrg/image-builder/master/image-builder.py
	chmod +x tools/image-builder.py

# Get the boot config creator tool
tools/tomatboot-config.py:
	mkdir -p tools
	cd tools && wget https://raw.githubusercontent.com/TomatOrg/tomatboot-config/master/tomatboot-config.py
	chmod +x tools/tomatboot-config.py

#########################
# Running in QEMU
#########################

#QEMU_PATH ?= qemu-system-x86_64
QEMU_PATH = ~/Documents/coding/qemu/bin/debug/native/x86_64-softmmu/qemu-system-x86_64

# Run qemu
qemu: bin/tomatos.img
	$(QEMU_PATH) \
		-drive if=pflash,format=raw,readonly,file=tools/OVMF.fd \
		-netdev tap,id=mynet0,ifname=tap0,script=no,downscript=no \
		-device rtl8139,netdev=mynet0 \
		-drive file=bin/tomatos.img,media=disk,format=raw \
		-no-reboot -no-shutdown \
		-machine q35 $(QEMU_ARGS)

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

clean: tomatboot-uefi-clean
	rm -rf build bin

# Delete all the tools
clean-tools:
	rm -rf tools
