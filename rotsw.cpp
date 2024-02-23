#include "rotsw.h"

RotSW::RotSW(uint press_pin0, uint rot_A_pin0, uint rot_B_pin0) :
press_pin(press_pin0), rot_A_pin(rot_A_pin0), rot_B_pin(rot_B_pin0) {
    gpio_pull_up(press_pin);
    gpio_init(press_pin);
    gpio_init(rot_A_pin);
    gpio_init(rot_B_pin);

    already_pressed = false;
    A_state = gpio_get(rot_A_pin);
}

// condition checks debounce input
bool RotSW::pressed() {
    bool curr_pressed = !gpio_get(press_pin);
    if (curr_pressed && !already_pressed) {
        return (already_pressed = true);
    } else if (!curr_pressed && already_pressed) {
        already_pressed = false;
    }
    return false;
}

// condition checks account for rotations both ways
// with both "rising" and "falling edges"
direction RotSW::rotate() {
    bool curr_A_state = gpio_get(rot_A_pin);
    bool curr_B_state = gpio_get(rot_B_pin);

    if (curr_A_state != A_state) {
        A_state = curr_A_state;
        if (A_state) {
            return curr_B_state ? CLOCKWISE : COUNTER_CLOCKWISE;
        } else {
            return curr_B_state ? COUNTER_CLOCKWISE : CLOCKWISE;
        }
    }
    return NOWHERE;
}