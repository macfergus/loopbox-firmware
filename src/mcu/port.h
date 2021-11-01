#ifndef __LOOPBOX_PORT_H__
#define __LOOPBOX_PORT_H__

#include <cstdint>

// GPIO pin control are in memory mapped registers. The pins are organized
// in banks of 32. The bits in this structure control each individual pin
// See p 379 in datasheet for details
struct portctl {
    volatile uint32_t DIR;
    volatile uint32_t DIRCLR;
    volatile uint32_t DIRSET;
    volatile uint32_t DIRTGL;
    volatile uint32_t OUT;
    volatile uint32_t OUTCLR;
    volatile uint32_t OUTSET;
    volatile uint32_t OUTTGL;
    volatile uint32_t IN;
    volatile uint32_t CTRL;
    volatile uint32_t WRCONFIG;
    volatile uint32_t reserved;
    volatile uint8_t PMUX[16];
    volatile uint8_t PINCFG[32];
};
#define PORTA ((volatile struct portctl*)(0x41004400))
#define PORTB ((volatile struct portctl*)(0x41004480))

#endif
