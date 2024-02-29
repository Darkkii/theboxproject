#ifndef PICO_MODBUS_ROTSW_H
#define PICO_MODBUS_ROTSW_H

#include <cstdio>
#include <queue>
#include "pico/stdlib.h"

#define PRESS_DEBOUNCE_DELAY_US 50000

typedef enum RotSW_enum {
    NO_EVENT,
    PRESS,
    CLOCKWISE,
    COUNTER_CLOCKWISE,
    WEIRD
} RotSW_event;

class RotSW {
private:
    static uint mPressPin;
    static uint64_t mPrevRise; // used for debouncing stutter
    static uint mRotAPin;
    static uint mRotBPin;
    static std::queue<RotSW_event> mEvents;
public:
    explicit RotSW(uint press_pin0 = 12,
                   uint rot_A_pin0 = 11,
                   uint rot_B_pin0 = 10);
    static void interruptHandler(uint gpio,
                                 uint32_t event_mask);
    RotSW_event getEvent();
};

#endif //PICO_MODBUS_ROTSW_H
