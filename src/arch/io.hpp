#pragma once

#include <stdint.h>

namespace arch::io {

    void write8(uint16_t port, uint8_t data);
    void write16(uint16_t port, uint16_t data);
    void write32(uint16_t port, uint32_t data);

    uint8_t read8(uint16_t port);
    uint16_t read16(uint16_t port);
    uint32_t read32(uint16_t port);

}
