########################################################################################################################
# Build config
########################################################################################################################

# Should debug be enabled
DEBUG 		?= 1

# Should qemu debugger be enabled
DEBUGGER 	?= 0

# Qemu acceleration
QEMU_ACCEL	?= 0

# Prefix to the compiler
PREFIX 		?=

########################################################################################################################
# Build constants
########################################################################################################################

CC 			:= $(PREFIX)clang
LD			:= $(PREFIX)ld.lld

CFLAGS 		:= -Wall -Werror -Wno-unused-label
CFLAGS  	+= -target x86_64-unknown-elf
CFLAGS 		+= -mno-sse -mno-sse2 -mno-mmx -mno-80387 -m64
CFLAGS 		+= -mno-red-zone -fno-builtin -mcmodel=kernel
CFLAGS 		+= -march=nehalem -fcoroutines-ts
CFLAGS 		+= -fstack-protector -ffreestanding -fno-pic
CFLAGS 		+= -O2 -flto -g
CFLAGS 		+= -Ikernel

LDFLAGS 	:= -nostdlib -no-pie
LDFLAGS 	+= --oformat elf_amd64
LDFLAGS 	+= -z max-page-size=0x1000
LDFLAGS 	+= -T kernel/linker.ld

SRCS		:= $(shell find kernel -name '*.c')

ifeq ($(DEBUG), 1)
	CFLAGS += -D__TOMATOS_DEBUG__
	CFLAGS += -fsanitize=undefined

	BIN_DIR := bin/DEBUG
	BUILD_DIR := build/DEBUG
else
	BIN_DIR := bin/RELEASE
	BUILD_DIR := build/RELEASE
endif

########################################################################################################################
# Phony
########################################################################################################################

.PHONY: default qemu image clean clean-all

default: image

########################################################################################################################
# Targets
########################################################################################################################

OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:%.o=%.d)
BINS ?=
-include $(DEPS)

$(BIN_DIR)/tomatos.elf: $(BINS) $(OBJS)
	@echo LD $@
	@mkdir -p $(@D)
	@$(LD) $(LDFLAGS) -o $@ $(OBJS)

$(BUILD_DIR)/%.c.o: %.c
	@echo CC $@
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -MMD -D__FILENAME__="\"$<\"" -D__MODULE__="\"$(notdir $(basename $<))\"" -c $< -o $@

$(BUILD_DIR)/%.asm.o: %.asm
	@echo NASM $@
	@mkdir -p $(@D)
	@nasm -g -i $(BUILD_DIR) -F dwarf -f elf64 -o $@ $<

include makefiles/image.mk

clean:
	rm -rf build

clean-all: clean
	rm -rf bin
