#include "./serial.h"

void read_chip_serial(uint32_t* out) {
    out[0] = *((volatile uint32_t*)(0x0080a00c));
    out[1] = *((volatile uint32_t*)(0x0080a040));
    out[2] = *((volatile uint32_t*)(0x0080a044));
    out[3] = *((volatile uint32_t*)(0x0080a048));
}
