#ifndef TOMATKERNEL_STDDEF_H
#define TOMATKERNEL_STDDEF_H

#include "stdint.h"

#define NULL ((void*)0)
#define offsetof(st, mem) __builtin_offsetof(st, mem)

// use the builtin types for the size, uintptr and ptddiff
typedef __SIZE_TYPE__       size_t;
typedef int64_t             ssize_t;
typedef __UINTPTR_TYPE__    uintptr_t;
typedef __INTPTR_TYPE__     intptr_t;
typedef __PTRDIFF_TYPE__    ptrdiff_t;


#define SIZE_MAX            (__SIZE_MAX__)
#define UINTPTR_MAX         (__UINTPTR_MAX__)
#define INTPTR_MAX         (__INTPTR_MAX__)
#define PTRDIFF_MAX         (__PTRDIFF_MAX__)

#ifdef __cplusplus
namespace std {

    typedef decltype(nullptr) nullptr_t;
    typedef ::size_t size_t;

}
#endif

#endif //TOMATKERNEL_STDDEF_H
