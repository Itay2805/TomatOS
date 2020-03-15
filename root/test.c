#include <stdint.h>
#include <stddef.h>

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

int64_t __syscall2(uint64_t number, int64_t arg1, int64_t arg2) {
    int64_t ret = 0;
    int64_t error = 0;
    asm volatile ("syscall"
    : "=a"(ret), "=b"(error)
    : "a"(number), "D"(arg1), "S"(arg2)
    : "rcx", "r11");
    if (error != 0) {
        return -error;
    }
    return ret;
}

int64_t __syscall3(uint64_t number, int64_t arg1, int64_t arg2, int32_t arg3) {
    int64_t ret = 0;
    int64_t error = 0;
    asm volatile ("syscall"
    : "=a"(ret), "=b"(error)
    : "a"(number), "D"(arg1), "S"(arg2), "d"(arg3)
    : "rcx", "r11");
    if (error != 0) {
        return -error;
    }
    return ret;
}


static void sys_log(const char* str) {
    __syscall1(0x00, (uintptr_t) str);
}

static int vfs_resolve(const char* path, const char** out) {
    return __syscall2(0x50, (uintptr_t)path, (uintptr_t)out);
}

static int fs_open(int fs, const char* path) {
    return __syscall2(0x60, fs, (uintptr_t)path);
}

static int file_read(int file, void* buffer, size_t len) {
    return __syscall3(0x70, file, (uintptr_t)buffer, len);
}

void _start() {

    // resolve the path and open it
    // TODO: library function to just open a file
    const char* out_path = 0;
    int fs = vfs_resolve("/hello.txt", &out_path);
    if (fs < 0) goto cleanup;
    int file = fs_open(fs, out_path);
    if (file < 0) goto cleanup;

    // read it
    char buffer[255] = {0};
    int res = file_read(file, buffer, sizeof(buffer));
    if (res < 0) goto cleanup;

    // log it
    sys_log(buffer);

cleanup:
    sys_log("Got error in syscall");
    while(1);
}
