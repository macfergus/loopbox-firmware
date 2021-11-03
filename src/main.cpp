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
    MIDILight lights[] = {
        {PORTA, 8, 24, MIDILight::Polarity::note_off_light_on},
        {PORTA, 9, 25, MIDILight::Polarity::note_off_light_on},
        {PORTA, 10, 26, MIDILight::Polarity::note_off_light_on},
        {PORTA, 11, 27, MIDILight::Polarity::note_off_light_on},

        {PORTA, 12, 28, MIDILight::Polarity::note_on_light_on},
        {PORTA, 13, 29, MIDILight::Polarity::note_on_light_on},
        {PORTA, 14, 30, MIDILight::Polarity::note_on_light_on},
        {PORTA, 15, 31, MIDILight::Polarity::note_on_light_on},

        {PORTA, 16, 32, MIDILight::Polarity::note_on_light_on},
        {PORTA, 17, 33, MIDILight::Polarity::note_on_light_on},
        {PORTA, 18, 34, MIDILight::Polarity::note_on_light_on},
        {PORTA, 19, 35, MIDILight::Polarity::note_on_light_on},
    };
    MIDILightBlock lightblock(lights);

    while (true) {
        tud_task();
        midi.dispatch(lightblock);

        const uint32_t now = millis();
        for (auto& button : buttons) {
            button.process(now);
        }
    }
}
