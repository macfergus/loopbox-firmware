#ifndef __LOOPBOX_ARM_SYSTICK_H__
#define __LOOPBOX_ARM_SYSTICK_H__

#include <cstdint>

struct systickctl {
    volatile uint32_t CTRL;
    volatile uint32_t LOAD;
    volatile uint32_t VAL;
    volatile uint32_t CALIB;
};
#define SYSTICK ((volatile struct systickctl*)(0xe000e010))

// ctrl flags
const uint32_t SYSTICK_ENABLE = 0x01;
const uint32_t SYSTICK_INT_ENABLE = 0x02;
const uint32_t SYSTICK_USE_CPU_CLOCK = 0x04;

#endif
