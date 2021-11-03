#ifndef __LOOPBOX_MIDI_H__
#define __LOOPBOX_MIDI_H__

#include <cstdint>
#include <span>

#include "../mcu/samd.h"
#include "./debouncer.h"
#include "./led.h"

struct NoteOn {
    uint8_t note;
    uint8_t velocity;
};

struct NoteOff {
    uint8_t note;
};

class MIDI {
public:
    template<typename NoteListener>
    void dispatch(NoteListener listener) {
        uint8_t midi_packet[4];
        while (tud_midi_available()) {
            tud_midi_packet_read(midi_packet);
            // packet format: usbmidi id, midi id, note num, velo
            // Two different ways to make a note off:
            // explicit: midi event type 0x80 (usbmidi id 0x08); or
            // implicit: midi event type 0x90 (usbmidi id 0x09) and velocity 0
            const bool is_note_on =
                ((midi_packet[0] & 0x0f) == 0x09) &&
                (midi_packet[3] > 0);
            const bool is_note_off =
                ((midi_packet[0] & 0x0f) == 0x08) ||
                ((midi_packet[0] & 0x0f) == 0x09 && (midi_packet[3] == 0));
            if (is_note_on) {
                const auto note = midi_packet[2];
                const auto velocity = midi_packet[3];
                listener.handleNoteOn({note, velocity});
            } else if (is_note_off) {
                const auto note = midi_packet[2];
                listener.handleNoteOff({note});
            }
        }
    }

    void sendNoteOn(uint8_t note, uint8_t velocity);
    void sendNoteOff(uint8_t note);

private:
};

class MIDILight {
public:
    enum class Polarity { note_on_light_on, note_off_light_on };
    MIDILight(volatile portctl* port, uint8_t pin, uint8_t note, Polarity polarity);

    uint8_t note() const { return note_; }
    void handleNoteOn();
    void handleNoteOff();

private:
    LED led_;
    uint8_t note_;
    Polarity polarity_;
};

class MIDILightBlock {
public:
    MIDILightBlock(std::span<MIDILight> lights);

    void handleNoteOn(NoteOn event);
    void handleNoteOff(NoteOff event);

private:
    std::span<MIDILight> lights_;
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
