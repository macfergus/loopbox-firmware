#include "./led.h"

LED::LED(volatile portctl* port, uint32_t pin) :
    port_(port),
    pin_(pin) {

    port_->DIRSET = 1 << pin_;
    port_->OUTCLR = 1 << pin_;
}

void LED::turnOn() {
    port_->OUTSET = 1 << pin_;
}

void LED::turnOff() {
    port_->OUTCLR = 1 << pin_;
}
