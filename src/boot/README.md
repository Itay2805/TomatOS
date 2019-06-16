# TomatKernel Boot

In order to have support for multiple bootloaders, the kernel is going to expect a simple unified boot information 
structure. Each boot protocol will be implemented here, and will be compiled as needed.

## Supported protocols
### Multiboot 1
This is targeted to provide compatibility, mainly since qemu can load it directly with ease.

### Multiboot 2
This is targeted to allow the kernel to work with both UEFI and Legacy boot machines.

## Boot allocator
For ease of use there is a simple boot allocator that allows the boot compatability code allocate memory as it needs 
(does not support freeing memory!)