#include "./loopbox/led.h"
#include "./loopbox/systick.h"
#include "./loopbox/usb.h"
#include "./mcu/samd.h"

int main() {
    initUSBClock();
    initSystick(48000000);
    LED led(PORTA, 22);

    while (true) {
        led.turnOn();
        delayMillis(500);
        led.turnOff();
        delayMillis(500);
    }
}
