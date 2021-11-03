#include "./serial.h"

Serial read_chip_serial() {
    return {
        *((volatile uint32_t*)(0x0080a00c)),
        *((volatile uint32_t*)(0x0080a040)),
        *((volatile uint32_t*)(0x0080a044)),
        *((volatile uint32_t*)(0x0080a048))
    };
}
