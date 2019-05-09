//
// Created by Itay on 09/05/2019.
//

#include "klib.h"
#include <process/syscalls.h>

bool open(resource_descriptor_t* desc, resource_t* resource) {
    bool result;
    asm volatile ("int $0x80" : "=a"(result) : "a"(SYSCALL_OPEN), "D"(desc), "S"(resource));
    return result;
}

bool read(resource_t res, void* buffer, int size, int* outSize) {
    bool result;
    asm volatile ("int $0x80" : "=a"(result) : "a"(SYSCALL_READ), "D"(res), "S"(buffer), "d"(size), "c"(outSize));
    return result;
}

bool write(resource_t res, const void* buffer, int size, int* outSize) {
    bool result;
    asm volatile ("int $0x80" : "=a"(result) : "a"(SYSCALL_WRITE), "D"(res), "S"(buffer), "d"(size), "c"(outSize));
    return result;
}

bool seek(resource_t res, int relative, ptrdiff_t offset) {
    bool result;
    asm volatile ("int $0x80" : "=a"(result) : "a"(SYSCALL_SEEK), "D"(res), "S"(relative), "d"(offset));
    return result;
}

bool tell(resource_t res, uint64_t* offset) {
    bool result;
    asm volatile ("int $0x80" : "=a"(result) : "a"(SYSCALL_TELL), "D"(res), "S"(offset));
    return result;
}

bool close(resource_t res) {
    bool result;
    asm volatile ("int $0x80" : "=a"(result) : "a"(SYSCALL_CLOSE), "D"(res));
    return result;
}

bool invoke(resource_t res, int command, void* arg) {
    bool result;
    asm volatile ("int $0x80" : "=a"(result) : "a"(SYSCALL_INVOKE), "D"(res), "S"(command), "d"(arg));
    return result;
}

bool tkill(int tid) {
    bool result;
    asm volatile ("int $0x80" : "=a"(result) : "a"(SYSCALL_THREAD_KILL), "D"(tid));
    return result;
}

