########################################################################################################################
# Build config
########################################################################################################################

CC := clang
LD := ld.lld

ARCH ?= amd64

DEBUG ?= 0

########################################################################################################################
# Build constants
########################################################################################################################

CFLAGS := -Wall -Werror
CFLAGS += -ffreestanding -fno-pic
CFLAGS += -Ikernel
CFLAGS += -O2 -flto -g

ifeq ($(DEBUG), 1)
	CFLAGS += -fsanitize=undefined
endif

CFLAGS += -DPRINTF_DISABLE_SUPPORT_FLOAT

LDFLAGS := -nostdlib -no-pie

SRCS += kernel/debug/ubsan.c
SRCS += kernel/mem/mm.c
SRCS += kernel/mem/pmm.c
SRCS += kernel/mem/tlsf.c
SRCS += kernel/proc/event.c
SRCS += kernel/proc/handle.c
SRCS += kernel/proc/process.c
SRCS += kernel/sync/critical.c
SRCS += kernel/sync/lock.c
SRCS += kernel/util/except.c
SRCS += kernel/util/list.c
SRCS += kernel/util/printf.c
SRCS += kernel/util/stb_ds.c
SRCS += kernel/util/string.c

ifeq ($(DEBUG), 1)
#	SRCS +=
endif

ifeq ($(DEBUG), 1)
	BIN_DIR := bin/$(ARCH)/DEBUG
	BUILD_DIR := build/$(BUILD)/DEBUG
else
	BIN_DIR := bin/$(ARCH)/RELEASE
	BUILD_DIR := build/$(BUILD)/RELEASE
endif

include makefiles/$(ARCH)/consts.mk

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
-include $(DEPS)

$(BIN_DIR)/tomatos.elf: $(OBJS)
	@echo LD $@
	@mkdir -p $(@D)
	@$(LD) $(LDFLAGS) -o $@ $^

$(BUILD_DIR)/%.c.o: %.c
	@echo CC $@
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -MMD -D__FILENAME__="\"$<\"" -D__MODULE__="\"$(notdir $(basename $<))\"" -c $< -o $@

-include makefiles/$(ARCH)/image.mk

clean:
	rm -rf build

clean-all: clean
	rm -rf bin
