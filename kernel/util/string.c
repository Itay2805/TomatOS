#include "string.h"

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
    const unsigned char* p1 = ptr1;
    const unsigned char* p2 = ptr2;

    if (len != 0) {
        do {
            if (*p1++ != *p2++) {
                return (*--p1 - *--p2);
            }
        } while (--len != 0);
    }

    return (0);
}

void* memcpy(void* _dst, const void* _src, size_t len) {
    if (_dst != _src) {
        char* dst = _dst;
        const char* src = _src;
        while (len-- > 0) {
            *dst++ = *src++;
        }
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

void* memset(void* dst, int val, size_t size) {
    char* a = dst;
    while (size-- > 0) {
        *a++ = val;
    }
    return dst;
}
