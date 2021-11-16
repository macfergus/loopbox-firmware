import sys
import time

import mido


def note_on(note):
    return mido.Message('note_on', note=note, velocity=127)


def note_off(note):
    return mido.Message('note_off', note=note)


def wait_for(port, note, on):
    while True:
        msg = port.receive()
        print(msg)
        if msg.type in ('note_on', 'note_off') and msg.note == note:
            if on:
                if msg.type == 'note_on' and msg.velocity > 0:
                    return
            else:
                if msg.type == 'note_off' or msg.velocity == 0:
                    return 


def fail(msg):
    sys.stderr.write(msg + '\n')
    sys.exit(1)


def test_input(port):
    buttons = {
        24: 'Y1', 25: 'Y2', 26: 'Y3', 27: 'Y4',
        28: 'R1', 29: 'R2', 30: 'R3', 31: 'R4',
        32: 'B1', 33: 'B2', 34: 'B3', 35: 'B4',
    }
    for note in sorted(buttons.keys()):
        print(f'Press {buttons[note]}')
        wait_for(port, note, on=True)
        wait_for(port, note, on=False)


def test_output(port):
    # Turn everything off
    for note in range(24, 28):
        port.send(note_on(note))
    for note in range(28, 36):
        port.send(note_off(note))

    # Turn on mutes one at a time
    for note in range(24, 28):
        port.send(note_off(note))
        time.sleep(0.15)
        port.send(note_on(note))

    # Turn on others one at a time
    for note in range(28, 36):
        port.send(note_on(note))
        time.sleep(0.15)
        port.send(note_off(note))


def main():
    # Test input first
    print('Testing input...')
    if 'LoopBox' not in mido.get_input_names():
        fail('No MIDI input for LoopBox detected')
    with mido.open_input('LoopBox') as input_port:
        test_input(input_port)

    # Then output
    print('Testing output...')
    if 'LoopBox' not in mido.get_output_names():
        fail('No MIDI output for LoopBox detected')

    # Give the tester time to look at the lights
    time.sleep(0.5)
    with mido.open_output('LoopBox') as output:
        test_output(output)

    print('Done!')


if __name__ == '__main__':
    main()
