#ifndef __LOOPBOX_SYSTICK_H__
#define __LOOPBOX_SYSTICK_H__

#include "../mcu/samd.h"
#include "./systick.h"

volatile uint32_t systick_millis = 0;

// Configure systick to 1 tick per ms
void initSystick(uint32_t cpu_clock) {
    // Disable systick while configuring
    SYSTICK->CTRL = 0;
    SYSTICK->LOAD = (cpu_clock / 1000) - 1;
    SYSTICK->VAL = 0;
    SYSTICK->CTRL = (
        SYSTICK_ENABLE |
        SYSTICK_USE_CPU_CLOCK |
        SYSTICK_INT_ENABLE
    );
}

// Systick interrupt handler; must add this to the vector table
void handleSystick() {
    systick_millis = systick_millis + 1;
}

uint32_t millis() {
    return systick_millis;
}

void delayMillis(uint32_t time_to_wait) {
    const uint32_t now = millis();
    while (millis() - now < time_to_wait) {
    }
}

#endif
