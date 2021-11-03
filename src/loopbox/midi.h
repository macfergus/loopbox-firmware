#ifndef __LOOPBOX_MIDI_H__
#define __LOOPBOX_MIDI_H__

#include <cstdint>

#include "../mcu/samd.h"
#include "./debouncer.h"

class MIDI {
public:
    void sendNoteOn(uint8_t note, uint8_t velocity);
    void sendNoteOff(uint8_t note);
};

class MIDIButton {
public:
    MIDIButton(MIDI& midi, volatile portctl* port, uint8_t pin, uint8_t note);
    void process(uint32_t elapsed);

private:
    MIDI& midi_;
    Debouncer debouncer_;
    const uint8_t note_;
};

#endif
