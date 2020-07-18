
# TODO: support uefi boot

QEMU_ARGS += -hdd $^
QEMU_ARGS += -m 4G -smp 4
QEMU_ARGS += -machine q35
QEMU_ARGS += -debugcon stdio
QEMU_ARGS += -monitor telnet:localhost:4321,server,nowait
QEMU_ARGS += --no-shutdown
QEMU_ARGS += --no-reboot

ifeq ($(DEBUGGER), 1)
	QEMU_ARGS += -S -s
endif

#
# A target to start the kernel in qemu
#
qemu: $(BIN_DIR)/image.hdd
	qemu-system-x86_64 $(QEMU_ARGS) | python3 scripts/trace2funcs.py $(BIN_DIR)/tomatos.elf

#
# A target to build a bootable image
#
image: $(BIN_DIR)/image.hdd

#
# Builds the image itself
#
$(BIN_DIR)/image.hdd:	$(BIN_DIR)/tomatos.elf \
						makefiles/amd64/boot/qloader2.cfg \
						makefiles/amd64/boot/qloader2.bin
	@mkdir -p $(@D)
	@echo "Creating disk"
	@rm -rf $@
	@dd if=/dev/zero bs=1M count=0 seek=64 of=$@
	@echo "Creating echfs partition"
	@parted -s $@ mklabel msdos
	@parted -s $@ mkpart primary 1 100%
	@echfs-utils -m -p0 $@ quick-format 32768
	@echo "Importing files"
	@echfs-utils -m -p0 $@ import $(BIN_DIR)/tomatos.elf tomatos.elf
	@echfs-utils -m -p0 $@ import makefiles/amd64/boot/qloader2.cfg qloader2.cfg
	@echo "Installing qloader2"
	@makefiles/amd64/boot/qloader2-install makefiles/amd64/boot/qloader2.bin $@


