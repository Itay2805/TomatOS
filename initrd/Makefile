
CFLAGS := \
	-target x86_64-unknown-elf \
	-ffreestanding \
	-nostdlib \
	-nostdinc \
	-Wall \
	-Werror \
	-O3 \
	-g \
	-static


test.elf: test.c
	clang-9 $(CFLAGS) $^ -o $@
