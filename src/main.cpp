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
    initUSBPins();
    tusb_init();

    bool led_state = false;
    uint32_t last_change = millis();
    while (true) {
        tud_task();

        uint32_t now = millis();
        if (now - last_change >= 500) {
            if (led_state) {
                led.turnOff();
                led_state = false;
            } else {
                led.turnOn();
                led_state = true;
            }
            last_change = now;
        }
    }
}
