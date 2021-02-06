
# TODO: support uefi boot

QEMU_ARGS += -m 4G -smp 4
QEMU_ARGS += -machine q35
QEMU_ARGS += -debugcon stdio
QEMU_ARGS += -monitor telnet:localhost:4321,server,nowait
QEMU_ARGS += --no-shutdown
QEMU_ARGS += --no-reboot
QEMU_ARGS += -cpu Nehalem

# We require iommu
QEMU_ARGS += -device intel-iommu,aw-bits=48

ifeq ($(DEBUGGER), 1)
	QEMU_ARGS += -S -s
endif

ifeq ($(shell uname -r | sed -n 's/.*\( *Microsoft *\).*/\1/p'), Microsoft)
	QEMU := qemu-system-x86_64.exe
	ifeq ($(QEMU_ACCEL), 1)
		QEMU_ARGS += --accel whpx
	endif
else
	QEMU := qemu-system-x86_64
	ifeq ($(QEMU_ACCEL), 1)
		QEMU_ARGS += --enable-kvm
	endif
endif

#
# A target to start the kernel in qemu
#
qemu: $(BIN_DIR)/image.hdd
	$(QEMU) -hdd $^ $(QEMU_ARGS) | python3 scripts/trace2funcs.py $(BIN_DIR)/tomatos.elf

#
# A target to build a bootable image
#
image: $(BIN_DIR)/image.hdd

#
# Builds the image itself
#
$(BIN_DIR)/image.hdd:	$(BIN_DIR)/tomatos.elf \
						boot/limine.cfg
	@mkdir -p $(@D)
	@echo "Creating disk"
	@rm -rf $@
	dd if=/dev/zero bs=1M count=0 seek=64 of=$@
	@echo "Creating echfs partition"
	parted -s $@ mklabel msdos
	parted -s $@ mkpart primary 1 100%
	echfs-utils -m -p0 $@ quick-format 32768
	@echo "Importing files"
	echfs-utils -m -p0 $@ import $(BIN_DIR)/tomatos.elf tomatos.elf
	echfs-utils -m -p0 $@ import boot/limine.cfg limine.cfg
	@echo "Installing qloader2"
	tools/limine-install boot/limine.bin $@
