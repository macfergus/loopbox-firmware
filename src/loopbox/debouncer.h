#ifndef __LOOPBOX_DEBOUNCER_H__
#define __LOOPBOX_DEBOUNCER_H__

#include <cstdint>

#include "../mcu/samd.h"

class Debouncer {
public:
    enum class StateChange { no_change, did_rise, did_fall };

    Debouncer(volatile portctl* port, uint8_t pin);

    StateChange process(uint32_t now);

private:
    enum class State { low, rising, high, falling };
    State state_;
    volatile portctl* const port_;
    uint8_t pin_;
    uint32_t entered_state_;
};

#endif
