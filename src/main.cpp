#include <tusb.h>

#include "./loopbox/debouncer.h"
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

    Debouncer button{PORTA, 7};
    while (true) {
        tud_task();

        const uint32_t now = millis();
        const auto state = button.process(now);
        if (state == Debouncer::StateChange::did_rise) {
            led.turnOn();
        } else if (state == Debouncer::StateChange::did_fall) {
            led.turnOff();
        }
    }
}
