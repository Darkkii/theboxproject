#ifndef PICO_MODBUS_ROTSW_H
#define PICO_MODBUS_ROTSW_H

#include <cstdio>
#include <queue>

typedef enum RotSW_enum {
    NOTHING,
    PRESS,
    CLOCKWISE,
    COUNTER_CLOCKWISE,
} RotSW_event;

class RotSW {
private:
    static uint mPressPin;
    static uint mRotAPin;
    static uint mRotBPin;
    static std::queue<RotSW_event> mEvents;
public:
    explicit RotSW(uint press_pin0 = 12,
                   uint rot_A_pin0 = 11,
                   uint rot_B_pin0 = 10);
    static void InterruptHandler(uint gpio,
                                 uint32_t event_mask);
    explicit operator RotSW_event();
};

#endif //PICO_MODBUS_ROTSW_H
