#include "./loopbox/led.h"
#include "./loopbox/systick.h"
#include "./mcu/samd.h"

int main() {
    initSystick(1000000);
    LED led(PORTA, 22);

    while (true) {
        led.turnOn();
        delayMillis(500);
        led.turnOff();
        delayMillis(500);
    }
}
