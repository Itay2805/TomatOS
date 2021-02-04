# TomatOS
A 64bit tomato kernel

## Building

### Toolchain
First you would need to build a custom clang toolchain, the reason for this is that we use a patched version of the 
sources to allow c code to make use of the llvm coroutines builtins.

You can get the download the patched sources from [here](https://github.com/TomatOrg/llvm-project/tree/tomatos) (note 
how the branch is not master but tomatos), simply follow the normal guidelines of building clang, you will also need to 
build `ld.lld`.

### Kernel

To build the kernel image simply invoke make:
```
make PREFIX=<path to clang bin dir> -j
```

To create an image with limine:
```
make PREFIX=<path to clang bin dir> image -j
```

To create an image and run it in qemu:
```
make PREFIX=<path to clang bin dir> qemu -j
```

#### Make Options:
* `DEBUG=[1/0]` - allows to turn on/off debug mode (default is 1)
