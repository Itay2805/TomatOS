#ifndef __UTIL_SYMLIST_H__
#define __UTIL_SYMLIST_H__

#include <stddef.h>

typedef struct symlist_t {
    size_t addr;
    char *name;
} symbol_t;

extern struct symlist_t symlist[];

__attribute__((always_inline, unused))
static inline char* symlist_name_from_address(size_t* off, size_t addr) {
    for (size_t i = 0; ; i++) {
        if (symlist[i].addr >= addr) {
            *off = addr - symlist[i - 1].addr;
            return symlist[i - 1].name;
        }
    }
    *off = 0;
    return NULL;
}

#endif //__UTIL_SYMLIST_H__
