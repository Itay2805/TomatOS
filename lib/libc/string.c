#include "string.h"
#include "stdbool.h"

void* memcpy(void* destptr, const void* srcptr, size_t num) {
    uint8_t* dest = destptr;
    const uint8_t* src = srcptr;
    while(num--) {
        *dest++ = *src++;
    }
    return destptr;
}

void* memmove(void* destptr, const void* srcptr, size_t num) {
    uint8_t* dest = destptr;
    const uint8_t* src = srcptr;
    uint8_t tmp[num];
    memcpy(tmp, src, num);
    memcpy(dest, tmp, num);
    return dest;
}

int memcmp(const void* ptr1, const void* ptr2, size_t num) {
    const uint8_t* buf1 = ptr1;
    const uint8_t* buf2 = ptr2;
    while(num) {
        if(*buf1 != *buf2) {
            return *buf1 - *buf2;
        }
        buf1++;
        buf2++;
        num--;
    }

    return 0;
}

void* memchr(const void* ptr, int value, size_t num) {
    const uint8_t* buf = ptr;
    while(num) {
        if(*buf == (uint8_t)value) {
            return (void*)buf;
        }
        buf++;
    }
    return NULL;
}

void* memset(void* ptr, int value, size_t num) {
    uint8_t val8 = (uint8_t)(value);
    uint8_t* buf = ptr;
    while(num--) {
        *buf++ = val8;
    }
    return ptr;
}

char* strcpy(char* dest, const char* src) {
    char* ret = dest;
    while(*src) {
        *dest++ = *src++;
    }
    *dest = 0;
    return ret;
}

char* strncpy(char* dest, const char* src, size_t num) {
    char* ret = dest;
    while(num-- && *src) {
        *dest++ = *src++;
    }
    while(num--) {
        *dest++ = 0;
    }
    return ret;
}

char* strcat(char* dest, const char* src) {
    char* ret = dest;
    while(*dest++);
    strcpy(dest, src);
    return ret;
}

char* strncat(char* dest, const char* src, size_t num) {
    char* ret = dest;
    while(*dest++);
    strncpy(dest, src, num);
    return ret;
}

int strcmp(const char* str1, const char* str2) {
    while(*str1++ == *str2++ && *str1 != 0);
    return *(str1 - 1) - *(str2 - 1);
}

int strncmp(const char* str1, const char* str2, size_t num) {
    return memcmp(str1, str2, num);
}

char* strchr(const char* str, int character) {
    while(*str) {
        if(*str == character) {
            return (char*)str;
        }
        str++;
    }
    return NULL;
}

size_t strcspn(const char* str1, const char* str2) {
    size_t span = 0;
    const char* str2start = str2;
    while(*str1) {
        while(*str2) {
            if(*str1 == *str2) {
                return span;
            }
            str2++;
        }
        str2 = str2start;
        span++;
    }
    return span;
}

size_t strspn(const char* str1, const char* str2) {
    size_t count = 0;
    while(*str1) {
        bool found = false;
        while(*str2) {
            if(*str1 == *str2) {
                found = true;
                count++;
                str1++;
                break;
            }
            str2++;
        }
        if(!found) return count;
    }
    return count;
}

char* strstr(const char* str1, const char* str2) {
    while(*str1) {
        if(strcmp(str1, str2) == 0) {
            return (char*)str1;
        }
        str1++;
    }
    return NULL;
}

size_t strlen(const char* str) {
    const char* start = str;
    while(*str) {
        str++;
    }
    return str - start;
}
