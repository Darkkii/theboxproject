#include <iostream>
#include "PicoSW.h"

using namespace std;

void PicoSW::interruptHandler(uint gpio, uint32_t event_mask)
{
    if (gpio == SW_ROT_A_PIN) {
        gpio = gpio_get(SW_ROT_B_PIN) ? SW_ROT_A_PIN : SW_ROT_B_PIN;
    }
    mEvents.emplace(gpio, event_mask, time_us_32());
}

PicoSW::PicoSW(bool rot_rot, bool rot_sw, bool sw_0, bool sw_1, bool sw_2)
{
    mPrev_SW_Rise = 0;

    if (rot_rot) {
        gpio_init(SW_ROT_B_PIN);
        gpio_set_irq_enabled_with_callback(SW_ROT_A_PIN,
                                           GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
                                           true,
                                           interruptHandler);
    }

    if (rot_sw) {
        gpio_pull_up(SW_ROT_SW_PIN);
        gpio_init(SW_ROT_SW_PIN);
        gpio_set_irq_enabled_with_callback(SW_ROT_SW_PIN,
                                           GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
                                           true,
                                           interruptHandler);
    }

    if (sw_0) {
        gpio_pull_up(SW_0_PIN);
        gpio_init(SW_0_PIN);
        gpio_set_irq_enabled_with_callback(SW_0_PIN,
                                           GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
                                           true,
                                           interruptHandler);
    }

    if (sw_1) {
        gpio_pull_up(SW_1_PIN);
        gpio_init(SW_1_PIN);
        gpio_set_irq_enabled_with_callback(SW_1_PIN,
                                           GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
                                           true,
                                           interruptHandler);
    }

    if (sw_2) {
        gpio_pull_up(SW_2_PIN);
        gpio_init(SW_2_PIN);
        gpio_set_irq_enabled_with_callback(SW_2_PIN,
                                           GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
                                           true,
                                           interruptHandler);
    }
}

// Initialize static member
queue<event_tuple> PicoSW::mEvents;

PicoSW_event PicoSW::getEvent()
{
    if (mEvents.empty()) {
        return NO_EVENT;
    }

    uint gpio;
    uint32_t event_mask;
    uint32_t time_stamp;
    tie(gpio, event_mask, time_stamp) = mEvents.front();
    mEvents.pop();

    switch (gpio) {
        case SW_ROT_A_PIN:
            if (time_stamp - mPrev_SW_Rise >= PRESS_DEBOUNCE_DELAY_US) {
                switch (event_mask) {
                    case GPIO_IRQ_EDGE_RISE:
                        return CLOCKWISE;
                    case GPIO_IRQ_EDGE_FALL:
                        return COUNTER_CLOCKWISE;
                }
            }
            break;
        case SW_ROT_B_PIN:
            if (time_stamp - mPrev_SW_Rise >= PRESS_DEBOUNCE_DELAY_US) {
                switch (event_mask) {
                    case GPIO_IRQ_EDGE_RISE:
                        return COUNTER_CLOCKWISE;
                    case GPIO_IRQ_EDGE_FALL:
                        return CLOCKWISE;
                }
            }
            break;
        case SW_2_PIN:
        case SW_1_PIN:
        case SW_0_PIN:
        case SW_ROT_SW_PIN:
            switch (event_mask) {
                case GPIO_IRQ_EDGE_RISE:
                    mPrev_SW_Rise = time_stamp;
                    break;
                case GPIO_IRQ_EDGE_FALL:
                    if (time_stamp - mPrev_SW_Rise >= PRESS_DEBOUNCE_DELAY_US) {
                        return static_cast<PicoSW_event>(gpio);
                    }
            }
            break;
    }
    return NO_EVENT;
}