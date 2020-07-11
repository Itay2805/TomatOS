
# TODO: support uefi boot

#
# A target to start the kernel in qemu
#
qemu: $(BIN_DIR)/image.hdd
	qemu-system-x86_64 -hdd $^ --enable-kvm -m 4G -smp 4 -machine q35 -debugcon stdio

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


