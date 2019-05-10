#ifndef COMMON_H
#define COMMON_H

////////////////////////////////////////////////////////////////////////////
// Common
////////////////////////////////////////////////////////////////////////////
//
// Basically has common macros and stuff
//
////////////////////////////////////////////////////////////////////////////

#define MIN(x, y) ((x) <= (y) ? (x) : (y))

#define MAX(x, y) ((x) >= (y) ? (x) : (y))

#define CLAMP_MAX(x, max) MIN(x, max)

#define CLAMP_MIN(x, min) MAX(x, min)

#define IS_POW2(x) (((x) != 0) && ((x) & ((x)-1)) == 0)

#define ALIGN_DOWN(n, a) ((n) & ~((a) - 1))

#define ALIGN_UP(n, a) ALIGN_DOWN((n) + (a) - 1, (a))

#define ALIGN_DOWN_PTR(p, a) ((void *)ALIGN_DOWN((uintptr_t)(p), (a)))

#define ALIGN_UP_PTR(p, a) ((void *)ALIGN_UP((uintptr_t)(p), (a)))

#define KB(x) (((uint64_t)x) * 1024u)

#define MB(x) (((uint64_t)x) * KB(1024))

#define GB(x) (((uint64_t)x) * MB(1024))

#define TB(x) (((uint64_t)x) * GB(1024))

#define UNUSED(x) ((void)x)

static inline void pause() {
    asm volatile ("pause" ::: "memory");
}

#endif