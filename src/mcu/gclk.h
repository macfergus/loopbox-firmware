#ifndef __MCU_GCLK_H__
#define __MCU_GCLK_H__

#include <cstdint>

struct gclkctl {
    volatile uint8_t CTRL;
    volatile uint8_t STATUS;
    volatile uint16_t CLKCTRL;
    volatile uint32_t GENCTRL;
    volatile uint32_t GENDIV;
};
#define GCLK ((volatile struct gclkctl*)(0x40000c00))
const uint8_t GCLK_STATUS_SYNCBUSY = 1 << 7;
const uint32_t GCLK_GENCTRL_ID_GEN0 = 0;
const uint32_t GCLK_GENCTRL_SRC_DFLL48M = 7 << 8;
const uint32_t GCLK_GENCTRL_GENEN = 1 << 16;
const uint32_t GCLK_GENCTRL_IDC = 1 << 17;

const uint16_t GCLK_CLKCTRL_CLKEN = 1 << 14;
const uint16_t GCLK_CLKCTRL_GEN0 = 0 << 8;
const uint16_t GCLK_CLKCTRL_ID_USB = 6 << 0;

#endif
