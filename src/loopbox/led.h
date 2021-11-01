#ifndef __LOOPBOX_LED_H__
#define __LOOPBOX_LED_H__

#include "../mcu/samd.h"

class LED {
public:
    LED(volatile portctl* port, uint32_t pin);

    void turnOn();
    void turnOff();

private:
    volatile portctl* port_;
    uint32_t pin_;
};

#endif
