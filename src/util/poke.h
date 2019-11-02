#ifndef TOMATKERNEL_POKE_H
#define TOMATKERNEL_POKE_H

#define POKE(type, addr) (*((volatile type*)(addr)))
#define POKE8(addr) POKE(uint8_t, addr)
#define POKE16(addr) POKE(uint16_t, addr)
#define POKE32(addr) POKE(uint32_t, addr)
#define POKE64(addr) POKE(uint64_t, addr)


#define RELAXED_POKE(type, addr) (*((type*)(addr)))
#define RELAXED_POKE8(addr) RELAXED_POKE(uint8_t, addr)
#define RELAXED_POKE16(addr) RELAXED_POKE(uint16_t, addr)
#define RELAXED_POKE32(addr) RELAXED_POKE(uint32_t, addr)
#define RELAXED_POKE64(addr) RELAXED_POKE(uint64_t, addr)

#endif //TOMATKERNEL_POKE_H
