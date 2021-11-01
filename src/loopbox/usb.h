#ifndef __LOOPBOX_USB_H__
#define __LOOPBOX_USB_H__

// Set up the DFLL at 48 MHz in USB recovery mode; use the 48 MHz DFLL as both
// the system clock and the USB clock
void initUSBClock();

#endif
