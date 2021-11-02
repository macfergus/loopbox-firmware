#include <tusb.h>

#include "./loopbox/led.h"
#include "./loopbox/systick.h"
#include "./loopbox/usb.h"
#include "./mcu/samd.h"

int main() {
    initUSBClock();
    initSystick(48000000);
    LED led(PORTA, 22);

    usbReset();
    usbEnable();
    tusb_init();

    while (true) {
        tud_task();

        led.turnOn();
        delayMillis(500);
        led.turnOff();
        delayMillis(500);
    }
}
