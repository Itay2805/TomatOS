#!/usr/bin/env bash

mkdir iso
mkdir iso/boot
mkdir iso/boot/grub
cp cmake-build-debug/TomatKernel iso/boot/TomatKernel
echo 'set timeout=0' >> iso/boot/grub/grub.cfg
echo 'set default=0' >> iso/boot/grub/grub.cfg
echo 'menuentry "TomatOS" {' >> iso/boot/grub/grub.cfg
echo '    multiboot /boot/TomatKernel' >> iso/boot/grub/grub.cfg
echo '    boot' >> iso/boot/grub/grub.cfg
echo '}' >> iso/boot/grub/grub.cfg
grub-mkrescue -o ./tomatos.iso ./iso
rm -rf iso