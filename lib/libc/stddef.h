#ifndef TOMATKERNEL_STDDEF_H
#define TOMATKERNEL_STDDEF_H

#include "stdint.h"

#define NULL (0)
#define offsetof(st, mem) __builtin_offsetof(st, mem)

typedef uint64_t            size_t;
typedef uint64_t            uintptr_t;
typedef int64_t             ptrdiff_t;

#define SIZE_MAX            ((size_t)UINT64_MAX)
#define UINTPTR_MAX         ((size_t)UINT64_MAX)
#define PTRDIFF_MAX         ((size_t)INT64_MAX)

#endif //TOMATKERNEL_STDDEF_H
