########################################################################################################################
# Build config
########################################################################################################################

# Which architecture to use
ARCH 		?= amd64

# Should debug be enabled
DEBUG 		?= 0

# Should qemu debugger be enabled
DEBUGGER 	?= 0

########################################################################################################################
# Build constants
########################################################################################################################

CC 			:= clang-10
LLC			:= llc-10
LD			:= ld.lld

CFLAGS 		:= -Wall -Werror
CFLAGS 		+= -Wno-unused-label
CFLAGS 		+= -fstack-protector
CFLAGS 		+= -ffreestanding -fno-pic
CFLAGS 		+= -Ikernel
CFLAGS 		+= -O2 -flto -g

LDFLAGS 	:= -nostdlib -no-pie

SRCS		:= $(shell find kernel -path kernel/arch -prune -false -o -name '*.c')
SRCS 		+= $(shell find kernel/arch/$(ARCH) -name '*.c')

ifeq ($(DEBUG), 1)
	CFLAGS += -D__TOMATOS_DEBUG__
	CFLAGS += -fsanitize=undefined

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
