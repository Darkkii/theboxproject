//
// Created by ronin on 23.2.2024.
//

#ifndef PICO_MODBUS_ROTSW_H
#define PICO_MODBUS_ROTSW_H

#include <cstdio>
#include <hardware/gpio.h>

typedef enum rotation_enum {
    NOWHERE,
    CLOCKWISE,
    COUNTER_CLOCKWISE
} direction;

class RotSW {
private:
    uint press_pin;
    uint rot_A_pin;
    uint rot_B_pin;
    bool already_pressed;
    bool A_state;
public:
    explicit RotSW(uint press_pin0 = 12,
                   uint rot_A_pin0 = 11,
                   uint rot_B_pin0 = 10);
    bool pressed();
    direction rotate();
};


#endif //PICO_MODBUS_ROTSW_H
