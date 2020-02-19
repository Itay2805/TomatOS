#include <stddef.h>

void* memset(void *dst, int c, size_t n) {
    char *q = dst;

    while (n--) {
		*q++ = c;
	}

    return dst;
}

int memcmp(const void *s1, const void *s2, size_t n) {
    const unsigned char *c1 = s1, *c2 = s2;
    int d = 0;

    while (n--) {
        d = (int)*c1++ - (int)*c2++;
        if (d)
            break;
    }

    return d;
}

void* memcpy(void* dst, const void* src, size_t n) {
    const char *p = src;
    char *q = dst;

    while (n--) {
		*q++ = *p++;
	}

    return dst;
}

void* memmove(void* dst, const void* src, size_t n) {
    const char *p = src;
    char *q = dst;
    if (q < p) {
		while (n--) {
			*q++ = *p++;
		}
	} else {
		p += n;
		q += n;
		while (n--) {
			*--q = *--p;
		}
	}

    return dst;
}

int strcmp(const char *s1, const char *s2) {
    const unsigned char *c1 = (const unsigned char *)s1;
    const unsigned char *c2 = (const unsigned char *)s2;
    unsigned char ch;
    int d = 0;

    while (1) {
        d = (int)(ch = *c1++) - (int)*c2++;
        if (d || !ch)
            break;
    }

    return d;
}

size_t strlen(const char *s) {
    const char *ss = s;
    while (*ss)
        ss++;
    return ss - s;
}
