#ifndef __MCU_SAMD_USB_H__
#define __MCU_SAMD_USB_H__

#include <cstdint>

struct usbctl {
    volatile uint8_t CTRLA;
    volatile uint8_t reserved1;
    volatile uint8_t SYNCBUSY;
    volatile uint8_t QOSCTRL;
    volatile uint32_t reserved2;
    volatile uint16_t CTRLB;
    volatile uint8_t DADD;
    volatile uint8_t reserved3;
    volatile uint8_t STATUS;
    volatile uint8_t FSMSTATUS; // 0x0d
    volatile uint16_t reserved4; // 0x0e
    volatile uint16_t FNUM; // 0x10
    volatile uint8_t reserved5; // 0x12
    volatile uint16_t INTENCLR; // 0x14
    volatile uint16_t reserved6; // 0x16
    volatile uint16_t INTENSET; // 0x18
    volatile uint16_t reserved7; // 0x1a
    volatile uint16_t INTFLAG; // 0x1c
    volatile uint16_t reserved8; // 0x1e
    volatile uint16_t EPINTSMRY; // 0x20
    volatile uint16_t reserved9; // 0x22
    volatile uint32_t DESCADD; // 0x24
    volatile uint16_t PADCAL; // 0x28
};
#define USB ((volatile struct usbctl*)(0x41005000))
const uint8_t USB_CTRLA_SWRST = 1 << 0;
const uint8_t USB_CTRLA_ENABLE = 1 << 1;
const uint8_t USB_SYNCBUSY_SWRST = 1 << 0;
const uint8_t USB_SYNCBUSY_ENABLE = 1 << 1;

#endif
