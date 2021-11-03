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
    MIDIButton buttons[] = {
        {midi, PORTA, 0, 24},
        {midi, PORTA, 1, 25},
        {midi, PORTA, 6, 26},
        {midi, PORTA, 7, 27},
        {midi, PORTA, 27, 28},
        {midi, PORTA, 28, 29},
        {midi, PORTB, 2, 30},
        {midi, PORTB, 3, 31},
        {midi, PORTA, 20, 32},
        {midi, PORTA, 21, 33},
        {midi, PORTB, 22, 34},
        {midi, PORTB, 23, 35},
    };

    while (true) {
        tud_task();

        const uint32_t now = millis();
        for (auto& button : buttons) {
            button.process(now);
        }
    }
}
