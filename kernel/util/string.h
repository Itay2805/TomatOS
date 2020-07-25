#ifndef __TOMATOS_KERNEL_UTIL_STRING_H__
#define __TOMATOS_KERNEL_UTIL_STRING_H__

#include <stddef.h>

int strcmp(const char* str1, const char* str2);
size_t strlen(const char* str);

#define memmove __builtin_memmove
#define memcpy __builtin_memcpy
#define memcmp __builtin_memcmp
#define memset __builtin_memset

#endif //__TOMATOS_KERNEL_UTIL_STRING_H__
