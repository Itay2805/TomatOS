########################################################################################################################
# Build config
########################################################################################################################

CC := clang
LD := ld.lld

ARCH ?= amd64

DEBUG ?= 0

DEBUGGER ?= 0

########################################################################################################################
# Build constants
########################################################################################################################

CFLAGS :=  -Wall -Werror
CFLAGS += -ffreestanding -fno-pic
CFLAGS += -Ikernel
CFLAGS += -O2 -flto -g

ifeq ($(DEBUG), 1)
	CFLAGS += -fsanitize=undefined
	CFLAGS += -D__TOMATOS_DEBUG__
endif

CFLAGS += -DPRINTF_DISABLE_SUPPORT_FLOAT

LDFLAGS := -nostdlib -no-pie

SRCS += kernel/acpi/tables/rsdp.c
SRCS += kernel/acpi/tables/rsdt.c
SRCS += kernel/acpi/tables/table.c
SRCS += kernel/acpi/acpi.c
SRCS += kernel/mem/mm.c
SRCS += kernel/mem/pmm.c
SRCS += kernel/mem/tlsf.c
SRCS += kernel/proc/event.c
SRCS += kernel/proc/handle.c
SRCS += kernel/proc/process.c
SRCS += kernel/proc/scheduler.c
SRCS += kernel/sync/critical.c
SRCS += kernel/sync/lock.c
SRCS += kernel/sys/pci/pci.c
SRCS += kernel/util/except.c
SRCS += kernel/util/list.c
SRCS += kernel/util/printf.c
SRCS += kernel/util/stb_ds.c
SRCS += kernel/util/string.c

# include lai
CFLAGS += -Ideps/lai/include
SRCS += deps/lai/core/error.c
SRCS += deps/lai/core/eval.c
SRCS += deps/lai/core/exec.c
SRCS += deps/lai/core/exec-operand.c
SRCS += deps/lai/core/libc.c
SRCS += deps/lai/core/ns.c
SRCS += deps/lai/core/object.c
SRCS += deps/lai/core/opregion.c
SRCS += deps/lai/core/os_methods.c
SRCS += deps/lai/core/variable.c
SRCS += deps/lai/core/vsnprintf.c
SRCS += deps/lai/helpers/pci.c
SRCS += deps/lai/helpers/pm.c
SRCS += deps/lai/helpers/resource.c
SRCS += deps/lai/helpers/sci.c

ifeq ($(DEBUG), 1)
	SRCS += kernel/debug/ubsan.c
endif

ifeq ($(DEBUG), 1)
	BIN_DIR := bin/$(ARCH)/DEBUG
	BUILD_DIR := build/$(ARCH)/DEBUG
else
	BIN_DIR := bin/$(ARCH)/RELEASE
	BUILD_DIR := build/$(ARCH)/RELEASE
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
BINS ?=
-include $(DEPS)

$(BIN_DIR)/tomatos.elf: $(BINS) $(OBJS)
	@echo LD $@
	@mkdir -p $(@D)
	@$(LD) $(LDFLAGS) -o $@ $(OBJS)

-include makefiles/$(ARCH)/targets.mk

$(BUILD_DIR)/%.c.o: %.c
	@echo CC $@
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -MMD -D__FILENAME__="\"$<\"" -D__MODULE__="\"$(notdir $(basename $<))\"" -c $< -o $@

-include makefiles/$(ARCH)/image.mk

clean:
	rm -rf build

clean-all: clean
	rm -rf bin
