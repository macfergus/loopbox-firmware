#include "./debouncer.h"

const uint32_t DEBOUNCE_TIME = 10;

Debouncer::Debouncer(volatile portctl* port, uint8_t pin) :
    state_(State::low),
    port_(port),
    pin_(pin) {

    port_->DIRCLR = (1 << pin_);
    port_->PINCFG[pin_] = PORT_PINCFG_INEN;
}

Debouncer::StateChange Debouncer::process(uint32_t now) {
    const bool is_high = port_->IN & (1 << pin_);
    if (state_ == State::low) {
        if (is_high) {
            state_ = State::rising;
            entered_state_ = now;
        }
    } else if (state_ == State::rising) {
        if (!is_high) {
            state_ = State::low;
        } else if (entered_state_ + DEBOUNCE_TIME >= now) {
            state_ = State::high;
            return StateChange::did_rise;
        }
    } else if (state_ == State::high) {
        if (!is_high) {
            state_ = State::falling;
            entered_state_ = now;
        }
    } else if (state_ == State::falling) {
        if (is_high) {
            state_ = State::high;
        } else if (entered_state_ + DEBOUNCE_TIME >= now) {
            state_ = State::low;
            return StateChange::did_fall;
        }
    }
    return StateChange::no_change;
}
