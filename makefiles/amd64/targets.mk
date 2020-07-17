
$(BUILD_DIR)/%.asm.o: %.asm
	@echo NASM $@
	@mkdir -p $(@D)
	@nasm -g -i $(BUILD_DIR) -F dwarf -f elf64 -o $@ $<

$(BUILD_DIR)/%.bin: %.real
	@echo NASM $@
	@mkdir -p $(@D)
	@nasm -f bin -o $@ $<

