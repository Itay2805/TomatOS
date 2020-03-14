#include <stdint.h>

int64_t __syscall1(uint64_t number, int64_t arg1) {
    int64_t ret = 0;
    int64_t error = 0;
    asm volatile ("syscall"
        : "=a"(ret), "=b"(error)
        : "a"(number), "D"(arg1)
        : "rcx", "r11");
    if (error != 0) {
        return -error;
    }
    return ret;
}

void _start() {

    __syscall1(0, (int64_t)"Hello World from userspace!");

    while(1);
}
