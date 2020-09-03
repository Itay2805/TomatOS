########################################################################################################################
# Build config
########################################################################################################################

# Which compiler and linker to use
CC := clang
LD := ld.lld

# Which architecture to use
ARCH ?= amd64

# Should debug be enabled
DEBUG ?= 0

# Should early console be enabled
EARLY_CONSOLE ?= $(DEBUG)

# Should qemu console be enabled
QEMU_CONSOLE ?= $(DEBUG)

# Should qemu debugger be enabled
DEBUGGER ?= 0

########################################################################################################################
# Build constants
########################################################################################################################

CFLAGS :=  -Wall -Werror
CFLAGS += -ffreestanding -fno-pic
CFLAGS += -Ikernel
CFLAGS += -O2 -flto -g

CFLAGS += -DPRINTF_DISABLE_SUPPORT_FLOAT

LDFLAGS := -nostdlib -no-pie

SRCS += kernel/acpi/tables/rsdp.c
SRCS += kernel/acpi/tables/rsdt.c
SRCS += kernel/acpi/tables/table.c
SRCS += kernel/acpi/acpi.c
SRCS += kernel/driver/pci/pci.c
SRCS += kernel/driver/pci/pciname.c
SRCS += kernel/driver/driver.c
SRCS += kernel/mem/mm.c
SRCS += kernel/mem/pmm.c
SRCS += kernel/mem/tlsf.c
SRCS += kernel/proc/event.c
SRCS += kernel/proc/handle.c
SRCS += kernel/proc/process.c
SRCS += kernel/proc/scheduler.c
SRCS += kernel/sync/critical.c
SRCS += kernel/sync/lock.c
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
	CFLAGS += -fsanitize=undefined
	CFLAGS += -D__TOMATOS_DEBUG__

	SRCS += kernel/debug/ubsan.c
endif

ifeq ($(EARLY_CONSOLE), 1)
	CFLAGS += -D__TOMATOS_EARLY_CONSOLE__

	SRCS += kernel/debug/early_console.c
endif

ifeq ($(QEMU_CONSOLE), 1)
	CFLAGS += -D__TOMATOS_QEMU_CONSOLE__
endif

ifeq ($(DEBUG), 1)
	BIN_DIR := bin/$(ARCH)/DEBUG
	BUILD_DIR := build/$(ARCH)/DEBUG
else
	BIN_DIR := bin/$(ARCH)/RELEASE
	BUILD_DIR := build/$(ARCH)/RELEASE
endif

include makefiles/$(ARCH)/consts.mk

#
# Will define a driver to be compiled given the attribute is set
#
define def-driver
	ifeq ($$(DRIVER_$$(shell echo $(1)_$(2) | tr a-z A-Z)), 1)
		DRIVER_SRCS :=
		include drivers/$(1)/$(2)/driver.mk
		SRCS += $$(addprefix drivers/$(1)/$(2)/,$$(DRIVER_SRCS))
	endif
endef

#
# Iterate and define all drivers
#
$(foreach file, $(wildcard drivers/disk/*), $(eval $(call def-driver,disk,$(notdir $(file))));)
$(foreach file, $(wildcard drivers/block/*), $(eval $(call def-driver,block,$(notdir $(file))));)
$(foreach file, $(wildcard drivers/fs/*), $(eval $(call def-driver,fs,$(notdir $(file))));)

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
