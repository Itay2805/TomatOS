# Compiler and Linker
CLANG := clang-9
CLANG_LD := ld.lld-9

default: bin/tomatos.img

#########################
# Sources and flag setup
#########################

# Sources, all c and assembly files basically
SRCS += $(shell find src -type f \( -name '*.c' ! -name 'symlist.c' \))
SRCS += $(shell find src -type f -name '*.asm')
SRCS += $(shell find lib/libc -type f -name '*.c')
SRCS += $(shell find lib/stb -type f -name '*.c')

# All the object files and build dirs
OBJS := $(SRCS:%=build/%.o)
DEPS := $(OBJS:%.o=%.d)
OBJDIRS := $(dir $(OBJS) $(BINS))

# Set the flags
COMMON_CFLAGS += \
	-target x86_64-unknown-elf \
	-masm=intel \
	-Wall \
	-Werror \
	-mno-sse \
	-mno-sse2 \
	-mno-mmx \
	-mno-80387 \
	-ffreestanding \
	-nostdlib \
	-nostdinc \
	-mno-red-zone \
	-mcmodel=kernel \
	-Wno-unused-label \
	-static \
	-g

# Set the linking flags
LDFLAGS += \
	--oformat elf_amd64 \
	--Bstatic \
	--nostdlib \
	-T scripts/linker.ld

NASMFLAGS += \
	-g \
	-F dwarf \
	-f elf64

# The kernel flags
KERNEL_CFLAGS = $(COMMON_CFLAGS)

# STB options
KERNEL_CFLAGS += -DSTB_SPRINTF_NOFLOAT

# Add kernel include dirs
KERNEL_CFLAGS += -Ilib/libc
KERNEL_CFLAGS += -Ilib/stb
KERNEL_CFLAGS += -Ilib/
KERNEL_CFLAGS += -Isrc/

#########################
# Compiling the kernel
#########################

# Include all deps
-include $(DEPS)

# Link everything
bin/image/tomatos.elf: $(BINS) $(OBJS)
	mkdir -p bin/image
	# generate fake symlist
	./scripts/generate_symbols.py > src/util/symlist.c
	$(CLANG) $(KERNEL_CFLAGS) -MMD -D __FILENAME__="\"$<\"" -c src/util/symlist.c -o build/src/util/symlist.c.o
	$(CLANG_LD) $(LDFLAGS) -o $@ $(OBJS) build/src/util/symlist.c.o
	# generate real symlist
	./scripts/generate_symbols.py "$@" > src/util/symlist.c
	$(CLANG) $(KERNEL_CFLAGS) -MMD -D __FILENAME__="\"$<\"" -c src/util/symlist.c -o build/src/util/symlist.c.o
	$(CLANG_LD) $(LDFLAGS) -o $@ $(OBJS) build/src/util/symlist.c.o

# build real mode asm code
build/%.bin: %.real
	@mkdir -p $(@D)
	nasm -f bin -o $@ $<

# Compile kernel c files
build/%.c.o: %.c
	@mkdir -p $(@D)
	$(CLANG) $(KERNEL_CFLAGS) -MMD -D __FILENAME__="\"$<\"" -c $< -o $@

# Compile kernel asm files
build/%.asm.o: %.asm
	@mkdir -p $(@D)
	nasm $(NASMFLAGS) -o $@ $<

#########################
# For testing
#########################

bin/image/EFI/BOOT/BOOTX64.EFI:
	$(MAKE) -C ./boot/ CLANG=$(CLANG)
	mkdir -p bin/image/EFI/BOOT/
	cp boot/bin/BOOTX64.EFI bin/image/EFI/BOOT/

bin/tomatos.img: \
		bin/image/EFI/BOOT/BOOTX64.EFI \
		bin/image/tomatos.elf
	cp ./tools/tomatboot.cfg ./bin/image/
	cp ./boot/bin/BOOTX64.EFI ./bin/image/
	cd bin && ../tools/image-builder/image-builder.py ../tools/image.yaml

#########################
# Clean
#########################

clean:
	rm -rf build bin
