#ifndef __MCU_PM_H__
#define __MCU_PM_H__

#include <cstdint>

struct pmctl {
    volatile uint8_t CTRL;
    volatile uint8_t SLEEP;
    volatile uint8_t reserved[6];
    volatile uint8_t CPUSEL;
    volatile uint8_t APBASEL;
    volatile uint8_t APBBSEL;
    volatile uint8_t APBCSEL;
    volatile uint8_t reserved2[8];
    volatile uint32_t AHBMASK;
    volatile uint32_t APBAMASK;
    volatile uint32_t APBBMASK;
    volatile uint32_t APBCMASK;
};
#define PM ((volatile struct pmctl*)(0x40000400))
const uint32_t PM_AHBMASK_USB_ENABLE = 1 << 6;
const uint32_t PM_APBBMASK_USB_ENABLE = 1 << 5;

#endif
