#ifndef TOMATKERNEL_COMMON_H
#define TOMATKERNEL_COMMON_H

#include <stdint.h>

////////////////////////////////////////////////////////////////////////////
// Memory manipulation
////////////////////////////////////////////////////////////////////////////

#define POKE(type, addr) *((type*)addr)
#define POKE8(addr) POKE(uint8_t, addr)
#define POKE16(addr) POKE(uint16_t, addr)
#define POKE32(addr) POKE(uint32_t, addr)
#define POKE64(addr) POKE(uint64_t, addr)

////////////////////////////////////////////////////////////////////////////
// Alignment
////////////////////////////////////////////////////////////////////////////

#define ALIGN_DOWN(n, a) ((n) & ~((a) - 1))
#define ALIGN_UP(n, a) ALIGN_DOWN((n) + (a) - 1, (a))
#define ALIGN_DOWN_PTR(p, a) ((void *)ALIGN_DOWN((uintptr_t)(p), (a)))
#define ALIGN_UP_PTR(p, a) ((void *)ALIGN_UP((uintptr_t)(p), (a)))

////////////////////////////////////////////////////////////////////////////
// Unit conversion
////////////////////////////////////////////////////////////////////////////

#define KB(x) (((uint64_t)x) * 1024u)
#define MB(x) (((uint64_t)x) * KB(1024))
#define GB(x) (((uint64_t)x) * MB(1024))
#define TB(x) (((uint64_t)x) * GB(1024))

////////////////////////////////////////////////////////////////////////////
// Attributes
////////////////////////////////////////////////////////////////////////////

#define PACKED __attribute__((packed))

////////////////////////////////////////////////////////////////////////////
// Kernel information
////////////////////////////////////////////////////////////////////////////

extern void* kernel_stack;
extern void* kernel_physical_end;
extern void* kernel_physical_start;
#define KERNEL_STACK ((uint64_t)&kernel_stack)

#define KERNEL_PHYSICAL_START ((uint64_t)&kernel_physical_start)
#define KERNEL_PHYSICAL_END ((uint64_t)&kernel_physical_end)

#endif //TOMATKERNEL_COMMON_H
