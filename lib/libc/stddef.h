#ifndef TOMATKERNEL_STDDEF_H
#define TOMATKERNEL_STDDEF_H

#include "stdint.h"

#define NULL (0)
#define offsetof(st, mem) __builtin_offsetof(st, mem)

// use the builtin types for the size, uintptr and ptddiff
typedef __SIZE_TYPE__       size_t;
typedef int64_t             ssize_t;
typedef __UINTPTR_TYPE__    uintptr_t;
typedef __PTRDIFF_TYPE__    ptrdiff_t;


#define SIZE_MAX            ((size_t)__SIZE_MAX__)
#define UINTPTR_MAX         ((size_t)__UINTPTR_MAX__)
#define PTRDIFF_MAX         ((size_t)__PTRDIFF_MAX__)

#endif //TOMATKERNEL_STDDEF_H
