#ifndef PICO_MODBUS_PICOSW_H
#define PICO_MODBUS_PICOSW_H

#include <cstdio>
#include <queue>
#include <tuple>
#include "pico/stdlib.h"

#define PRESS_DEBOUNCE_DELAY_US 100000

using namespace std;

typedef enum pico_sw_event_enum {
    NO_EVENT,
    SW_2_PRESS = 7,
    SW_1_PRESS = 8,
    SW_0_PRESS = 9,
    ROT_PRESS = 12,
    CLOCKWISE,
    COUNTER_CLOCKWISE,
} PicoSW_event;

typedef tuple<uint, uint32_t, uint32_t> event_tuple;

class PicoSW {
private:
    enum pico_sw_pin_enum {
        SW_2_PIN = 7,
        SW_1_PIN = 8,
        SW_0_PIN = 9,
        SW_ROT_B_PIN = 10,
        SW_ROT_A_PIN = 11,
        SW_ROT_SW_PIN = 12
    };

    uint32_t mPrev_SW_Rise;
    static queue<event_tuple> mEvents;
public:
    explicit PicoSW(bool rot_rot = false,
                    bool rot_sw = false,
                    bool sw_0 = false,
                    bool sw_1 = false,
                    bool sw_2 = false);
    static void interruptHandler(uint gpio, uint32_t event_mask);
    PicoSW_event getEvent();
};

#endif //PICO_MODBUS_PICOSW_H
