#include <tusb.h>

#include "./loopbox/led.h"
#include "./loopbox/midi.h"
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

    MIDI midi;
    MIDIButton button{midi, PORTA, 7, 27};

    while (true) {
        tud_task();

        const uint32_t now = millis();
        button.process(now);
    }
}
