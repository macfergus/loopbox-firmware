#include "samd.h"

#include "./led.h"
#include "./systick.h"

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
