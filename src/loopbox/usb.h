#ifndef __LOOPBOX_USB_H__
#define __LOOPBOX_USB_H__

/**
 * Set up the DFLL at 48 MHz in USB recovery mode; use the 48 MHz DFLL as both
 * the system clock and the USB clock
 */
void initUSBClock();

void usbReset();
void usbEnable();

/** Configure the USB D+/D- pins */
void initUSBPins();

/** USB interrupt handler */
void handleUSB();

#endif
