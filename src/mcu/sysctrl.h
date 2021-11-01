#ifndef __MCU_SYSCTRL_H__
#define __MCU_SYSCTRL_H__

#include <cstdint>

struct sysctrl {
    volatile uint32_t INTENCLR;
    volatile uint32_t INTENSET;
    volatile uint32_t INTFLAG;
    volatile uint32_t PCLKSR;
    volatile uint16_t XOSC;
    volatile uint16_t reserved;
    volatile uint16_t XOSC32K;
    volatile uint16_t reserved2;
    volatile uint32_t OSC32K;
    volatile uint8_t OSCULP32K;
    volatile uint8_t reserved3[3];
    volatile uint32_t OSC8M;
    volatile uint16_t DFLLCTRL;
    volatile uint16_t reserved4;
    volatile uint32_t DFLLVAL;
    volatile uint32_t DFLLMUL;
    volatile uint8_t DFLLSYNC;
    volatile uint8_t reserved5[3];
};
#define SYSCTRL ((volatile struct sysctrl*)(0x40000800))
const uint32_t SYSCTRL_PCLKSR_DFLLRDY = (1 << 4);
const uint16_t SYSCTRL_DFLLCTRL_MODE = (1 << 2);
const uint16_t SYSCTRL_DFLLCTRL_ENABLE = (1 << 1);
const uint32_t SYSCTRL_DFLLCTRL_USBCRM = (1 << 5);
const uint32_t SYSCTRL_DFLLCTRL_CCDIS = (1 << 8);

const uint8_t SYSCTRL_DFLLMUL_COARSE_STEP_OFFSET = 26;
const uint8_t SYSCTRL_DFLLMUL_FINE_STEP_OFFSET = 16;

const uint8_t SYSCTRL_DFLLVAL_COARSE_OFFSET = 10;
const uint8_t SYSCTRL_DFLLVAL_FINE_OFFSET = 0;

#endif
