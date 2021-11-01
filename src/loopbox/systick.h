#ifndef __LOOPBOX_SYSTICK_H__
#define __LOOPBOX_SYSTICK_H__

#include <cstdint>

// Enable systick at 1ms intervals
void initSystick(uint32_t cpu_clock);
// Interrupt handler; don't call this, add it to the vector table
void handleSystick();
// Return number of systicks counted since it was enabled
uint32_t millis();
// Busy wait until a number of millis have passed
void delayMillis(uint32_t time_to_wait);

#endif
