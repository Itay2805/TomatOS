#ifndef TOMATOS_STRING_H
#define TOMATOS_STRING_H

#include "defs.h"

size_t strlen(const char* s);
int strncmp(const char* str1, const char* str2, size_t n);
char* strncpy(char* restrict d, const char* restrict s, size_t n);

void memrev(void* ptr, int length);
void* memset(void* dest, int c, size_t n);
void* memcpy(void* restrict dest, const void* restrict src, size_t size);

#define strlen(s) __builtin_strlen(s)

#define memset(dest, c, n) __builtin_memset(dest, c, n)
#define memcpy(dest, src, size) __builtin_memcpy(dest, src, size)

#endif //TOMATOS_STRING_H
