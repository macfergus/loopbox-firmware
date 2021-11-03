#include <tusb.h>

#include "./midi.h"

void MIDI::sendNoteOn(uint8_t note, uint8_t velocity) {
    const uint8_t raw_midi[] = {0x90, note, velocity};
    tud_midi_stream_write(0, raw_midi, 3);
}

void MIDI::sendNoteOff(uint8_t note) {
    const uint8_t raw_midi[] = {0x90, note, 0};
    tud_midi_stream_write(0, raw_midi, 3);
}

MIDIButton::MIDIButton(
    MIDI& midi,
    volatile portctl* port,
    uint8_t pin,
    uint8_t note
) :
    midi_(midi),
    debouncer_(port, pin),
    note_(note) {}

void MIDIButton::process(uint32_t elapsed) {
    const auto state = debouncer_.process(elapsed);
    if (state == Debouncer::StateChange::did_rise) {
        midi_.sendNoteOn(note_, 127 /* velocity */);
    } else if (state == Debouncer::StateChange::did_fall) {
        midi_.sendNoteOff(note_);
    }
}
