#include <stddef.h>
#include <stdint.h>
#include "defs.h"

int strcmp(const char* str1, const char* str2) {
    while (*str1 && *str1 == *str2) {
        ++str1;
        ++str2;
    }
    return *str1 - *str2;
}

size_t strlen(const char* str) {
    const char *s = str;
    while (*s) {
        ++s;
    }
    return s - str;
}

int memcmp(const void* ptr1, const void* ptr2, size_t len) {
    const uint8_t* lhs_str = ptr1;
    const uint8_t* rhs_str = ptr2;

    for(size_t i = 0; i < len; i++) {
        if(lhs_str[i] < rhs_str[i])
            return -1;

        if(lhs_str[i] > rhs_str[i])
            return 1;
    }

    return 0;
}

void* memcpy(void* _dst, const void* _src, size_t len) {
    uint8_t* dst = _dst;
    const uint8_t* src = _src;

    while (len >= 8 * 8) {
        uint64_t w1; __builtin_memcpy(&w1, src, 8); src += 8;
        uint64_t w2; __builtin_memcpy(&w2, src, 8); src += 8;
        uint64_t w3; __builtin_memcpy(&w3, src, 8); src += 8;
        uint64_t w4; __builtin_memcpy(&w4, src, 8); src += 8;
        uint64_t w5; __builtin_memcpy(&w5, src, 8); src += 8;
        uint64_t w6; __builtin_memcpy(&w6, src, 8); src += 8;
        uint64_t w7; __builtin_memcpy(&w7, src, 8); src += 8;
        uint64_t w8; __builtin_memcpy(&w8, src, 8); src += 8;
        __builtin_memcpy(dst, &w1, 8); dst += 8;
        __builtin_memcpy(dst, &w2, 8); dst += 8;
        __builtin_memcpy(dst, &w3, 8); dst += 8;
        __builtin_memcpy(dst, &w4, 8); dst += 8;
        __builtin_memcpy(dst, &w5, 8); dst += 8;
        __builtin_memcpy(dst, &w6, 8); dst += 8;
        __builtin_memcpy(dst, &w7, 8); dst += 8;
        __builtin_memcpy(dst, &w8, 8); dst += 8;
        len -= 8 * 8;
    }
    if (len >= 4 * 8) {
        uint64_t w1; __builtin_memcpy(&w1, src, 8); src += 8;
        uint64_t w2; __builtin_memcpy(&w2, src, 8); src += 8;
        uint64_t w3; __builtin_memcpy(&w3, src, 8); src += 8;
        uint64_t w4; __builtin_memcpy(&w4, src, 8); src += 8;
        __builtin_memcpy(dst, &w1, 8); dst += 8;
        __builtin_memcpy(dst, &w2, 8); dst += 8;
        __builtin_memcpy(dst, &w3, 8); dst += 8;
        __builtin_memcpy(dst, &w4, 8); dst += 8;
        len -= 4 * 8;
    }
    if (len >= 2 * 8) {
        uint64_t w1; __builtin_memcpy(&w1, src, 8); src += 8;
        uint64_t w2; __builtin_memcpy(&w2, src, 8); src += 8;
        __builtin_memcpy(dst, &w1, 8); dst += 8;
        __builtin_memcpy(dst, &w2, 8); dst += 8;
        len -= 2 * 8;
    }
    if (len >= 8) {
        uint64_t w1; __builtin_memcpy(&w1, src, 8); src += 8;
        __builtin_memcpy(dst, &w1, 8); dst += 8;
        len -= 8;
    }
    if (len >= 4) {
        uint32_t w1; __builtin_memcpy(&w1, src, 4); src += 4;
        __builtin_memcpy(dst, &w1, 4); dst += 4;
        len -= 4;
    }
    if (len >= 2) {
        uint16_t w1; __builtin_memcpy(&w1, src, 2); src += 2;
        __builtin_memcpy(dst, &w1, 2); dst += 2;
        len -= 2;
    }
    if (len) {
        *dst = *src;
    }
    return _dst;
}

void* memmove(void* _dst, const void* _src, size_t len) {
    char* dst = _dst;
    const char* src = _src;

    if (src == dst)
        return dst;

    if (src > dst) {
        while (len--) {
            *dst++ = *src++;
        }
    } else {
        dst += len - 1;
        src += len - 1;
        while (len--) {
            *dst-- = *src--;
        }
    }

    return _dst;
}

void* memset(void* _dst, int val, size_t size) {
    uint8_t* dst = _dst;

    uint64_t pattern = (uint64_t)val
                     | ((uint64_t)val << 8)
                     | ((uint64_t)val << 16)
                     | ((uint64_t)val << 24)
                     | ((uint64_t)val << 32)
                     | ((uint64_t)val << 40)
                     | ((uint64_t)val << 48)
                     | ((uint64_t)val << 56);

    while (size >= 8 * 8) {
        __builtin_memcpy(dst, &pattern, 8); dst += 8;
        __builtin_memcpy(dst, &pattern, 8); dst += 8;
        __builtin_memcpy(dst, &pattern, 8); dst += 8;
        __builtin_memcpy(dst, &pattern, 8); dst += 8;
        __builtin_memcpy(dst, &pattern, 8); dst += 8;
        __builtin_memcpy(dst, &pattern, 8); dst += 8;
        __builtin_memcpy(dst, &pattern, 8); dst += 8;
        __builtin_memcpy(dst, &pattern, 8); dst += 8;
        size -= 8 * 8;
    }
    if (size >= 4 * 8) {
        __builtin_memcpy(dst, &pattern, 8); dst += 8;
        __builtin_memcpy(dst, &pattern, 8); dst += 8;
        __builtin_memcpy(dst, &pattern, 8); dst += 8;
        __builtin_memcpy(dst, &pattern, 8); dst += 8;
        size -= 4 * 8;
    }
    if (size >= 2 * 8) {
        __builtin_memcpy(dst, &pattern, 8); dst += 8;
        __builtin_memcpy(dst, &pattern, 8); dst += 8;
        size -= 2 * 8;
    }
    if (size >= 8) {
        __builtin_memcpy(dst, &pattern, 8); dst += 8;
        size -= 8;
    }
    if (size >= 4) {
        __builtin_memcpy(dst, &pattern, 4); dst += 4;
        size -= 4;
    }
    if (size >= 2) {
        __builtin_memcpy(dst, &pattern, 2); dst += 2;
        size -= 2;
    }
    if (size) {
        *dst = pattern;
    }
    return _dst;
}
