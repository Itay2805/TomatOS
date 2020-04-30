#include <stdint.h>
#include <stddef.h>

uint64_t __syscall0(uint64_t number) {
    uint64_t ret = 0;
    asm volatile ("syscall"
    : "=a"(ret)
    : "a"(number)
    : "rcx", "r11");
    return ret;
}

uint64_t __syscall1(uint64_t number, int64_t arg1) {
    uint64_t ret = 0;
    asm volatile ("syscall"
        : "=a"(ret)
        : "a"(number), "D"(arg1)
        : "rcx", "r11");
    return ret;
}

uint64_t __syscall2(uint64_t number, int64_t arg1, int64_t arg2) {
    uint64_t ret = 0;
    asm volatile ("syscall"
    : "=a"(ret)
    : "a"(number), "D"(arg1), "S"(arg2)
    : "rcx", "r11");
    return ret;
}

uint64_t __syscall3(uint64_t number, int64_t arg1, int64_t arg2, int32_t arg3) {
    uint64_t ret = 0;
    asm volatile ("syscall"
    : "=a"(ret)
    : "a"(number), "D"(arg1), "S"(arg2), "d"(arg3)
    : "rcx", "r11");
    return ret;
}

static void sys_log(const char* str) {
    __syscall1(0x00, (uintptr_t) str);
}

static int vfs_open(const char* path) {
    return (int)(int64_t)__syscall1(0x50, (uintptr_t)path);
}

static int file_read(int file, void* buffer, size_t len) {
    return (int)(int64_t)__syscall3(0x70, file, (uintptr_t)buffer, len);
}

static int create_event() {
    return (int)(int64_t)__syscall0(0x42);
}

static int set_timer(int event, int type, uint64_t trigger_time) {
    return (int)(int64_t)__syscall3(0x43, event, type, trigger_time);
}

static int wait_for_event(int number, int* events, int* index) {
    return (int)(int64_t)__syscall3(0x44, number, (uint64_t)events, (uint64_t)index);
}

void _start() {

    int timer = create_event();
    if (timer < 0) goto cleanup;
    if (set_timer(timer, 1, 1000) != 0) goto cleanup;

    while (1) {
        if(wait_for_event(1, &timer, NULL) != 0) goto cleanup;
        sys_log("Got timer event!");
    }

cleanup:
    sys_log("Got error in syscall");
    while(1);
}
