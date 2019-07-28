#ifndef TOMATKERNEL_COMMON_H
#define TOMATKERNEL_COMMON_H

#include <stdint.h>

////////////////////////////////////////////////////////////////////////////
// Memory manipulation
////////////////////////////////////////////////////////////////////////////

#define POKE(type, addr) (*((volatile type*)addr))
#define POKE8(addr) POKE(uint8_t, addr)
#define POKE16(addr) POKE(uint16_t, addr)
#define POKE32(addr) POKE(uint32_t, addr)
#define POKE64(addr) POKE(uint64_t, addr)

#define ADDROF(symbol) ((uintptr_t)&symbol)

////////////////////////////////////////////////////////////////////////////
// Alignment
////////////////////////////////////////////////////////////////////////////

#define ALIGN_DOWN(n, a) (((uint64_t)n) & ~((a) - 1ul))
#define ALIGN_UP(n, a) ALIGN_DOWN(((uint64_t)n) + (a) - 1ul, (a))
#define ALIGN_PAGE_DOWN(n) ALIGN_DOWN(n, KB(4))
#define ALIGN_PAGE_UP(n) ALIGN_UP(n, KB(4))

////////////////////////////////////////////////////////////////////////////
// Unit conversion
////////////////////////////////////////////////////////////////////////////

#define KB(x) (((uint64_t)x) * 1024ul)
#define MB(x) (((uint64_t)x) * KB(1024ul))
#define GB(x) (((uint64_t)x) * MB(1024ul))
#define TB(x) (((uint64_t)x) * GB(1024ul))

////////////////////////////////////////////////////////////////////////////
// Attributes
////////////////////////////////////////////////////////////////////////////

#define PACKED __attribute__((packed))

////////////////////////////////////////////////////////////////////////////
// Kernel information
////////////////////////////////////////////////////////////////////////////

extern void* kernel_physical_end;
extern void* kernel_physical_start;

#define KERNEL_PHYSICAL_START ((uint64_t)&kernel_physical_start)
#define KERNEL_PHYSICAL_END ((uint64_t)&kernel_physical_end)

#define BIT(n) (1 << n)
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#endif //TOMATKERNEL_COMMON_H
