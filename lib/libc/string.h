#ifndef __LIBC_STRING_H__
#define __LIBC_STRING_H__

void* memset(void *dst, int c, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);
void* memcpy(void* dst, const void* src, size_t n);
void* memmove(void* dst, const void* src, size_t n);

int strcmp(const char *s1, const char *s2);
size_t strlen(const char *s);

#endif //__LIBC_STRING_H__
