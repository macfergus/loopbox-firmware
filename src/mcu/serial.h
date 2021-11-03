#ifndef __MCU_SERIAL_H__
#define __MCU_SERIAL_H__

#include <cstdint>

struct Serial {
    uint32_t word1;
    uint32_t word2;
    uint32_t word3;
    uint32_t word4;
};

Serial read_chip_serial();

#endif
