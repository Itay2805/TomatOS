#ifndef __TOMATOS_KERNEL_UTIL_STRING_H__
#define __TOMATOS_KERNEL_UTIL_STRING_H__

#include <stddef.h>

int strcmp(const char* str1, const char* str2);
size_t strlen(const char* str);

int memcmp(const void* ptr1, const void* ptr2, size_t len);
void* memcpy(void* dst, const void* src, size_t len);
void* memmove(void* dst, const void* src, size_t len);
void* memset(void* dst, int val, size_t len);

#endif //__TOMATOS_KERNEL_UTIL_STRING_H__
